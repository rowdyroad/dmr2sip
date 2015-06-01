#pragma once

namespace Commutator {

class Program : public PointHandler
{
    private:
        Commutator::Storage& storage_;
        std::vector<std::shared_ptr<std::thread>> pool_;
        std::vector<Commutator::PointPtr> points_;
        std::vector<Commutator::Storage::Route> routes_;

        Commutator::Storage::Route* current_route_;

        Commutator::PointPtr getPointById(size_t point_id)
        {
            for (auto p : points_) {
    	        if (p->getConfiguration().point_id == point_id) {
        	    	    return p;
        		}
    	    }
    	    return Commutator::PointPtr();
        }

    public:
        Program(Commutator::Storage& storage, std::map<std::string, std::shared_ptr<PointFactory>>& factories)
            : storage_(storage)
            , routes_(storage.GetRoutes())
            , current_route_(nullptr)
        {
            storage.UpdateAllPointsStatus(Commutator::Storage::Point::Status::psInvactive);
            std::cout << "Initialize points" << std::endl;
            for (auto& point : storage.GetPoints()) {
                std::cout << "\tPoint id: " << point.point_id << std::endl
                          << "\tIdentity: " << point.id << std::endl
                          << "\tPassword: " << point.password << std::endl;
                Commutator::PointPtr p = factories.at(point.type)->Create(point, this);
                points_.push_back(p);
                storage.UpdatePointStatus(point.point_id, Commutator::Storage::Point::Status::psActive);
                std::shared_ptr<std::thread> thread(new std::thread(&Commutator::Point::Run, p));
                pool_.push_back(thread);
            }
        }

        ~Program()
        {
            Stop();
        }

        void Run()
        {
            for (auto thread: pool_) {
                thread->join();
            }
            storage_.UpdateAllPointsStatus(Commutator::Storage::Point::Status::psInvactive);
        }

        void Stop()
        {
            for (auto p : points_) {
                p->Stop();
            }
        }

        void OnCallReceived(Commutator::Point* const point, const std::string& number)
        {
	       std::cout << "RECEIVED point:" << point->getConfiguration().point_id << " number:" << number << std::endl;
	       current_route_ = nullptr;
            for (auto& route : routes_) {
                std::cout << " CHECK point: " << route.source_point_id << " number: " << route.source_number << std::endl;
                if (route.source_point_id == point->getConfiguration().point_id && (route.source_number == "*" || route.source_number == number)) {
                    std::cout << "\t\thas route to " << route.destination_point_id  << std::endl;
                    auto destination = this->getPointById(route.destination_point_id);
                    if (destination) {
	                   std::cout << "\t\thas destination" << std::endl;
                        current_route_ = &route;
                        storage_.addEvent(route.route_id, number);
                        destination->Initiate(route.destination_number);
                    break;
                    }
                }
                break;
            }

        	if (!current_route_) {
        	    storage_.addEvent(0, point->getConfiguration().name + " " + number);
        	}
        }

        void OnCallEnded(Commutator::Point* const point)
        {
            std::cout << "ENDED point: " << point->getConfiguration().point_id << std::endl;
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
}