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
#include "Debug.h"

#include "json.hh"

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
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        exit(1);
    }

    JSON::Value config = parse_file(argv[1]);
    signal(SIGINT, signalHandler);
    signal(SIGHUP, signalRestartHandler);
    Debug debugger("main");

    factories.insert(std::make_pair("sip", PointFactoryPtr(new Commutator::SIPPointFactory(config["sip"]["config_file"].as_string()))));
    factories.insert(std::make_pair("dmr", PointFactoryPtr(new Commutator::DMRPointFactory(config["dmr"]["authorization_key"].as_string(), config["dmr"]["delta"].as_int()))));

    storage.reset(new Commutator::Storage(  config["database"]["address"].as_string(),
                                            config["database"]["name"].as_string(),
                                            config["database"]["username"].as_string(),
                                            config["database"]["password"].as_string()));
    while (!quit) {
        debugger << "Starting..." << std::endl;
        try {
            program.reset(new Commutator::Program(*storage, factories));
            program->Run();
        } catch (std::exception& e) {
            debugger << "Catched exception: " << e.what() << std::endl;
        }
        debugger << "Wait for 3 seconds to restart..." << std::endl;
        sleep(3);
    }
    return 0;
}