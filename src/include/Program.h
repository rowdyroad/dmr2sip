#pragma once

#include <mutex>

namespace Commutator {

class Program : public PointHandler
{
    private:

	class Link {
	    private:
		static std::vector<Commutator::PointPtr> points_;
		Commutator::Storage::Route route_;
        	Commutator::PointPtr source_;
		Commutator::PointPtr destination_;
		std::string number_;
	    public:
		static void SetPoints(const std::vector<Commutator::PointPtr>& points)
		{
		    points_ = points;
		}

		Link(const Commutator::Storage::Route& route, const std::string& number)
		    : route_(route)
		{
		    for (auto p : Link::points_) {
			if (p->getConfiguration().point_id == route.source_point_id) {
			    source_ = p;
			} else if (p->getConfiguration().point_id == route.destination_point_id) {
			    destination_ = p;
			}
			if (source_ && destination_) break;
		    }
		}

		const Commutator::Storage::Route& getRoute() const { return route_; }
		const Commutator::PointPtr& getSource() const { return source_; }
		const Commutator::PointPtr& getDestination() const { return destination_; }
		const std::string& getNumber() const { return number_; }

		bool Connected() { return source_ && destination_; }
	};
	std::mutex mutex_;
        Commutator::Storage& storage_;
        std::vector<std::shared_ptr<std::thread>> pool_;
        std::vector<Commutator::PointPtr> points_;
        std::vector<Commutator::Storage::Route> routes_;
	std::map<size_t, std::shared_ptr<Link>> linked_points_;

    public:
        Program(Commutator::Storage& storage, std::map<std::string, std::shared_ptr<PointFactory>>& factories)
            : storage_(storage)
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
	    routes_ = storage_.GetRoutes();
	    std::cout << "Routes" << std::endl;
	    for (auto& route : routes_) {
		std::cout << "\tRoute id: " << route.route_id << std::endl
                          << "\tRoute: " << route.source_point_id << "(" << route.source_number << ") -> " << route.destination_point_id << "(" << route.destination_number << ")" << std::endl;

	    }
	    Link::SetPoints(points_);
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
	    {
		std::unique_lock<std::mutex> lock(mutex_);
		if (linked_points_.find(point->getConfiguration().point_id) != linked_points_.end()) {
		    std::cout << "\tIgnore incomming call, point in actived route now" << std::endl;
		    return;
		}
	    }
	    bool link_created = false;
            for (auto route : routes_) {
                std::cout << "\tCHECK point: " << route.source_point_id << " number: " << route.source_number << std::endl;
                if (route.source_point_id == point->getConfiguration().point_id && route.source_number == number) {
		    std::shared_ptr<Link> link(new Link(route, number));
		    if (link->Connected()) {
			std::cout << "\t\tLinked" << std::endl;
			{
			    std::unique_lock<std::mutex> lock(mutex_);
			    linked_points_.insert(std::make_pair(route.source_point_id, link));
			    linked_points_.insert(std::make_pair(route.destination_point_id, link));
                        }
			storage_.addEvent(route.route_id, number);
                        link->getDestination()->Initiate(route.destination_number);
			link_created = true;
			break;
		    }
		    break;
		}
            }

    	    if (!link_created) {
        	storage_.addEvent(0, point->getConfiguration().name + " " + number);
    	    }
        }

        void OnCallEnded(Commutator::Point* const point)
        {
	    std::cout << "ENDED point: " << point->getConfiguration().point_id << std::endl;

	    std::map<size_t, std::shared_ptr<Link>>::iterator link;
	    {
	    	std::unique_lock<std::mutex> lock(mutex_);
		link = linked_points_.find(point->getConfiguration().point_id);
		if (link == linked_points_.end()) {
		    std::cout << "\tNot found actived route for "<< point->getConfiguration().point_id << std::endl;
            	    return;
		}
	    }
	    
	    if (point == link->second->getDestination().get()) {
		std::cout << "Source hanguping" << std::endl;
		link->second->getSource()->Hangup();
		std::cout << "Source hangup successfully" << std::endl;
	    } else {
		std::cout << "Destination hanguping" << std::endl;
		link->second->getDestination()->Hangup();
		std::cout << "Destination hanguping  successfully" << std::endl;
	    }
	    {
		std::unique_lock<std::mutex> lock(mutex_);
		linked_points_.erase(link->second->getRoute().source_point_id);
		linked_points_.erase(link->second->getRoute().destination_point_id);
	    }
	    std::cout << "Call end successfully" << std::endl;
        }
    };
}