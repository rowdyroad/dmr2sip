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
    std::vector<std::thread> pool;
    std::cout << "Create Storage";
    Commutator::Storage storage(db);
    std::cout << "\t\t\t\t[ OK ]" << std::endl;
    std::cout << "Flush Point Status";
    storage.UpdateAllPointsStatus(Commutator::Storage::Point::Status::psInvactive);
    std::cout << "\t\t\t\t[ OK ]" << std::endl;

    typedef std::shared_ptr<Commutator::PointFactory> PointFactoryPtr;

    std::map<std::string, PointFactoryPtr> factories;

    factories.insert(std::make_pair("sip", PointFactoryPtr(new Commutator::SIPPointFactory())));
    factories.insert(std::make_pair("dmr", PointFactoryPtr(new Commutator::DMRPointFactory())));

    std::vector<Commutator::PointPtr> points;

    class Program : public Commutator::PointHandler
    {
    private:
        Commutator::Storage& storage_;
        std::vector<std::shared_ptr<std::thread>> pool_;
        std::vector<Commutator::PointPtr> points_;
        std::vector<Commutator::Storage::Route> routes_;

        Commutator::Storage::Route* current_route_;

        Commutator::PointPtr getPointById(size_t point_id)
        {
        for (auto& p : points_) {
            if (p->getConfiguration().point_id == point_id) {
                return p;
            }
        }
        return Commutator::PointPtr();
        }

    public:
        Program(Commutator::Storage& storage, std::map<std::string, PointFactoryPtr>& factories)
        : storage_(storage)
        , routes_(storage.GetRoutes())
        , current_route_(nullptr)
        {
        std::cout << "Initialize points" << std::endl;
        for (auto& point : storage.GetPoints()) {
            std::cout << "\tPoint id: " << point.point_id << std::endl
                      << "\tIdentity: " << point.id << std::endl
                      << "\tPassword: " << point.password << std::endl;
            Commutator::PointPtr p = factories.at(point.type)->Create(point, this);
                points_.push_back(p);
                storage.UpdatePointStatus(point.point_id, 1);
            std::shared_ptr<std::thread> thread(new std::thread(&Commutator::Point::Run, p));
            pool_.push_back(thread);
        }
        for (auto& thread: pool_) {
            thread->join();
        }
        storage.UpdateAllPointsStatus(Commutator::Storage::Point::Status::psInvactive);
        }

        void OnCallReceived(Commutator::Point* const point, const std::string& number)
        {
        for (auto& route : routes_) {
            if (route.source_point_id == point->getConfiguration().point_id && (route.source_number == "*" || route.source_number == number)) {
            auto destination = this->getPointById(route.destination_point_id);
            if (destination) {
                current_route_ = &route;
                storage_.addEvent(route.route_id, number);
                destination->Initiate(route.destination_number);
                break;
            }
            }
            break;
        }
        }

        void OnCallEnded(Commutator::Point* const point)
        {
        if (!current_route_) {
            std::cout << "Undefined current rout" << std::endl;
            return;
        }

        if (point->getConfiguration().point_id == current_route_->source_point_id) {
            auto destination = this->getPointById(current_route_->destination_point_id);
            if (destination) {
            destination->Hangup();
            }
        } else if (point->getConfiguration().point_id == current_route_->destination_point_id) {
            auto source = this->getPointById(current_route_->source_point_id);
            if (source) {
            source->Hangup();
            }
        }

        throw new Commutator::Exception(100, "Undefined point");
        }
    };


    Program(storage, factories);

    return 0;
}