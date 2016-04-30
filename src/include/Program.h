#pragma once

#include <mutex>
#include "Debug.h"

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
                        if (source_ && destination_) {
                            if (source_->Link(destination_) && destination_->Link(source_)) {
                                break;
                            }  else {
                                source_->UnLink(destination_);
                                destination_->UnLink(source_);
                                source_.reset();
                                destination_.reset();
                            }
                        }
                    }
                }
                ~Link()
                {
                    if (source_ && destination_) {
                        source_->UnLink(destination_);
                        destination_->UnLink(source_);
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
        Debug debugger_;

    public:
        Program(Commutator::Storage& storage, std::map<std::string, std::shared_ptr<PointFactory>>& factories)
            : storage_(storage)
            , debugger_("program")
        {
            storage.UpdateAllPointsStatus(Commutator::Storage::Point::Status::psInvactive);
            debugger_ << "Initialize points" << std::endl;
            for (auto& point : storage.GetPoints()) {
                debugger_ << "\tPoint id: " << point.point_id << std::endl
                          << "\tType: " << point.type << std::endl
                          << "\tName: " << point.name << std::endl;
                Commutator::PointPtr p = factories.at(point.type)->Create(point, this);
                points_.push_back(p);
                storage.UpdatePointStatus(point.point_id, Commutator::Storage::Point::Status::psActive);
                std::shared_ptr<std::thread> thread(new std::thread(&Commutator::Point::Run, p));
                pool_.push_back(thread);
            }
            routes_ = storage_.GetRoutes();
            debugger_ << "Routes" << std::endl;
            for (auto& route : routes_) {
                debugger_ << "\tRoute id: " << route.route_id << std::endl
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

        bool OnCallReceived(Commutator::Point* const point, const std::string& number)
        {
            debugger_ << "RECEIVED point:" << point->getConfiguration().point_id << " number:" << number << std::endl;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                if (linked_points_.find(point->getConfiguration().point_id) != linked_points_.end()) {
                    debugger_ << "\tIgnore incomming call, point in actived route now" << std::endl;
                    return false;
                }
            }
            bool link_created = false;
            for (auto route : routes_) {
                debugger_ << "\tCHECK point: " << route.source_point_id << " number: " << route.source_number << std::endl;
                if (route.source_point_id == point->getConfiguration().point_id && route.checkSourceNumber(number)) {
                    std::shared_ptr<Link> link(new Link(route, number));
                    if (link->Connected()) {
                        debugger_ << "\t\tLinked" << std::endl;
                        {
                            std::unique_lock<std::mutex> lock(mutex_);
                            linked_points_.insert(std::make_pair(route.source_point_id, link));
                            linked_points_.insert(std::make_pair(route.destination_point_id, link));
                        }
                        storage_.addCallEvent(route.route_id, number);
                        link->getDestination()->Initiate(route.destination_number);
                        link_created = true;
                        break;
                    }
                break;
                }
            }

            if (!link_created) {
                storage_.addCallEvent(0, point->getConfiguration().name + " " + number);
            }
            return link_created;
        }

        void OnCallEnded(Commutator::Point* const point)
        {
            debugger_ << "ENDED point: " << point->getConfiguration().point_id << std::endl;

            std::map<size_t, std::shared_ptr<Link>>::iterator link;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                link = linked_points_.find(point->getConfiguration().point_id);
                if (link == linked_points_.end()) {
                    debugger_ << "\tNot found actived route for "<< point->getConfiguration().point_id << std::endl;
                    return;
                }
            }

            bool callback = link->second->getRoute().phone_mode && !point->PhoneModeMaster();

            if (callback) {
                debugger_ << "Phone mode is on: need to callback" << std::endl;
                point->Callback();
            } else {
                debugger_ << "Ending call now" << std::endl;
                debugger_ << "\tHanguping remote point";
                (point == link->second->getSource().get()
                            ? link->second->getDestination()
                            : link->second->getSource())->Hangup();
                debugger_ << " Done" << std::endl;
                debugger_ << "\tDestroing link";
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    linked_points_.erase(link->second->getRoute().source_point_id);
                    linked_points_.erase(link->second->getRoute().destination_point_id);
                }
                debugger_ << " Done" << std::endl;
                debugger_ << "Call end successfully" << std::endl;
            }
        }
    };
}