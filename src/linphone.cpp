#include <string.h>
#include <string>
#include <stdio.h>
#include <signal.h>
#include <memory>
#include <thread>
#include <map>
#include "include/Exception.h"
#include "include/Configuration.h"
#include "include/Storage.h"
#include "include/Point.h"
#include "include/SIPPoint.h"
#include "include/DMRPoint.h"

volatile bool quit = false;


void signalHandler( int signum )
{
}


void signalRestartHandler(int signum)
{

}

int main(int argc, char*argv[])
{

    if (argc > 1) {
    	if (!strcmp(argv[1], "-d")) {
    	    auto devices = Commutator::SIPPoint::GetDevicesList();
    	    std::cout << "Devices list:" << std::endl;
    	    for (auto& p : devices) {
    		  std::cout << "\t" << p << std::endl;
    	    }
    	    return 0;
    	}
    }

    signal(SIGINT, signalHandler);
    signal(SIGHUP, signalRestartHandler);

    std::vector<std::thread> pool;
    Commutator::Configuration config("sip2dmr.conf");
    Commutator::Storage storage(config.getDbFilename());
    storage.UpdateAllPointsStatus(Commutator::Storage::Point::Status::psInvactive);


    typedef std::shared_ptr<Commutator::PointFactory> PointFactoryPtr;

    std::map<std::string, PointFactoryPtr> factories;

	factories.insert(std::make_pair("sip", PointFactoryPtr(new Commutator::SIPPointFactory())));
	factories.insert(std::make_pair("dmr", PointFactoryPtr(new Commutator::DMRPointFactory())));


    std::vector<Commutator::PointPtr> points;

    for (auto& point : storage.GetPoints()) {
        Commutator::PointPtr p = factories[point.type]->Create(point, nullptr);
        points.push_back(p);
        pool.push_back(std::thread([=]{
            p->Run();
        }));
    }



    for (auto& route: storage.GetRoutes()) {

    }

    for (auto& thread: pool) {
        thread.join();
    }

    return 0;
}