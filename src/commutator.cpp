#include <string.h>
#include <string>
#include <stdio.h>
#include <signal.h>
#include <memory>
#include <thread>
#include <map>
#include "include/Exception.h"
#include "include/Storage.h"
#include "include/Point.h"
#include "include/SIPPoint.h"
#include "include/DMRPoint.h"
#include "include/Program.h"

volatile bool quit = false;
std::unique_ptr<Commutator::Storage> storage;
std::unique_ptr<Commutator::Program> program;
std::map<std::string, PointFactoryPtr> factories;
typedef std::shared_ptr<Commutator::PointFactory> PointFactoryPtr;
factories.insert(std::make_pair("sip", PointFactoryPtr(new Commutator::SIPPointFactory())));
factories.insert(std::make_pair("dmr", PointFactoryPtr(new Commutator::DMRPointFactory())));

void signalHandler( int signum )
{
    if (program) {
        program->Stop();
    }
}


void signalRestartHandler(int signum)
{
    if (program) {
        program.reset(new Program(*storage, factories));
    }
}

int main(int argc, char*argv[])
{

    if (argc > 1) {
        if (!strcmp(argv[1], "-d")) {
            auto devices = Commutator::SIPPoint::GetDevicesList();
            std::cout << "Devices list:" << std::endl;
        size_t idx = 0;
            for (auto& p : devices) {
              std::cout << "\t" << idx++ << ". " << p << std::endl;
            }
            return 0;
        }
    }

    std::string db = "/tmp/dmr2sip.sqlite";
    for (size_t i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-f")) {
        db = argv[i+1];
        continue;
    }
    }

    signal(SIGINT, signalHandler);
    signal(SIGHUP, signalRestartHandler);
    storage.reset(new Commutator::Storage(db));
    program.reset(new Program(*storage, factories));
    program->Run();
    return 0;
}