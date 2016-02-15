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
typedef std::shared_ptr<Commutator::PointFactory> PointFactoryPtr;

std::vector<Commutator::PointPtr> Commutator::Program::Link::points_;

std::unique_ptr<Commutator::Storage> storage;
std::unique_ptr<Commutator::Program> program;
std::map<std::string, PointFactoryPtr> factories;

void signalHandler( int signum )
{
    std::cout << "Request for quit" << std::endl;
    if (program) {
        program->Stop();
    }
    quit = true;
}


void signalRestartHandler(int signum)
{
    std::cout << "Request for reload configuration" << std::endl;
    if (program) {
	program->Stop();
    }
}

int main(int argc, char*argv[])
{
        std::string server;
        std::string password;
        std::string username;
        std::string database;

    for (size_t i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-l")) {
            auto devices = Commutator::SIPPoint::GetDevicesList();
            std::cout << "Devices list:" << std::endl;
            size_t idx = 0;
            for (auto& p : devices) {
              std::cout << "\t" << idx++ << ". " << p << std::endl;
            }
            return 0;
        }
        if (!strcmp(argv[i], "-s")) {
            server = argv[i+1];
                continue;
        }
        if (!strcmp(argv[i], "-p")) {
            password = argv[i+1];
            continue;
        }
        if (!strcmp(argv[i], "-u")) {
            username = argv[i+1];
            continue;
        }
        if (!strcmp(argv[i], "-d")) {
            database = argv[i+1];
            continue;
        }
    }

    signal(SIGINT, signalHandler);
    signal(SIGHUP, signalRestartHandler);

    factories.insert(std::make_pair("sip", PointFactoryPtr(new Commutator::SIPPointFactory())));
    factories.insert(std::make_pair("dmr", PointFactoryPtr(new Commutator::DMRPointFactory())));

    storage.reset(new Commutator::Storage(server, database, username, password));
    while (!quit) {
        try {
            program.reset(new Commutator::Program(*storage, factories));
            program->Run();
        } catch (std::exception& e) {
            std::cout << "Catched exception: " << e.what() << std::endl;
        }
        std::cout << "Wait for 3 seconds to restart..." << std::endl;
        sleep(3);
    }
    return 0;
}