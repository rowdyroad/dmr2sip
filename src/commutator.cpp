#include <string.h>
#include <string>
#include <stdio.h>
#include <signal.h>
#include <memory>
#include <thread>
#include <map>
#include "Exception.h"
#include "Storage.h"
#include "Point.h"
#include "SIPPoint.h"
#include "DMRPoint.h"
#include "Program.h"

volatile bool quit = false;
typedef std::shared_ptr<Commutator::PointFactory> PointFactoryPtr;

std::vector<Commutator::PointPtr> Commutator::Program::Link::points_;

std::unique_ptr<Commutator::Storage> storage;
std::unique_ptr<Commutator::Program> program;
std::map<std::string, PointFactoryPtr> factories;

const char* dmrAuthorizationKey = "0x152C7E9D0x38BE41C70x71E96CA40x6CAC1AFC0x9E3779B9";
const size_t dmrAuthorizationDelta = 0x9E3779B9;

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
    std::string sip_config;

    if (argc < 2 || (argc - 1) % 2 != 0) {
        std::cout << "Usage: " << argv[0] << " <options>..." << std::endl;
        std::cout << "  -s Database server name (required)" << std::endl;
        std::cout << "  -u Database username (required)" << std::endl;
        std::cout << "  -p Database password (required)" << std::endl;
        std::cout << "  -d Database name (required)" << std::endl;
        std::cout << "  -l Linphone config filename (required)" << std::endl;
        return 1;
    }

    for (size_t i = 1; i < argc; i+=2) {

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

        if (!strcmp(argv[i], "-l")) {
            sip_config = argv[i+1];
            continue;
        }
    }

    signal(SIGINT, signalHandler);
    signal(SIGHUP, signalRestartHandler);

    factories.insert(std::make_pair("sip", PointFactoryPtr(new Commutator::SIPPointFactory(sip_config))));
    factories.insert(std::make_pair("dmr", PointFactoryPtr(new Commutator::DMRPointFactory(dmrAuthorizationKey, dmrAuthorizationDelta))));

    storage.reset(new Commutator::Storage(server, database, username, password));
    while (!quit) {
        std::cout << "Starting..." << std::endl;
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