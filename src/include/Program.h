#pragma once

#include <mutex>
#include "Debug.h"

namespace Commutator {

class Program : public PointHandler
{
    private:
        class Link {
            private:
                Commutator::Storage::Route route_;
                Commutator::PointPtr source_;
                Commutator::PointPtr destination_;
                std::string number_;
            public:
                Link(const Commutator::Storage::Route& route, const std::string& number, const std::list<Commutator::PointPtr>& points)
                    : route_(route)
                {
                    for (auto& p : points) {
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

        size_t replace_all(std::string &str, const std::string &from, const std::string &to)
        {
          size_t count = 0;

          size_t pos = 0;
          while ((pos = str.find(from, pos)) != std::string::npos) {
            str.replace(pos, from.length(), to);
            pos += to.length();
            ++count;
          }

          return count;
        }

        bool checkSourceNumber(const Commutator::Storage::Route& route, const std::string& number, std::string& destination)
        {
            JSON::Object nmbr = parse_string(number);
            std::cout << "Source number check: " << nmbr << std::endl;
            auto dst_map = route.destination_number;
            for (auto& kv : route.source_number) {
                std::string str;
                try {
                    str = nmbr[kv.first].as_string();
                } catch (...) {
                   str = "";
                }

                std::cout << "Rule for " << kv.first  << std::endl;

                if (kv.second.regex) {
                    std::cout << "\tchecking for regex - '" << str << "'" << std::endl;
                    std::list<std::string> matches;
                    regmatch_t match;
                    int error;
                    const char* bp = str.c_str();
                    size_t offset = 0;
                    while (!regexec(kv.second.regex.get(), bp + offset, 1, &match, REG_ICASE | REG_EXTENDED)) {
                        matches.push_back(str.substr(match.rm_so + offset, match.rm_eo - match.rm_so));
                        offset = match.rm_eo;
                    }

                    if (matches.empty()) {
                        std::cout << "\t\tNo matched" << std::endl;
                        return false;
                    }

                    size_t i = 1;
                    for (auto& match : matches) {
                        std::cout << "Match " << i << " " << match << std::endl;
                        auto it = kv.second.placeholders.find(i++);
                        if (it != kv.second.placeholders.end()) {
                            std::string placeholder = std::to_string(it->second);
                            placeholder = "\\" + placeholder;
                            std::cout << "placeholder " << placeholder << std::endl;
                            for (auto& kv : dst_map) {
                                std::cout << "replacing " << kv.second << " " << placeholder << "->" << match << std::endl;
                                replace_all(kv.second, placeholder, match);

                            }
                        }
                    }
                } else if (!kv.second.plain.empty()) {
                    std::cout << "\tchecking for plain - '" << str << "'" << std::endl;
                    if (kv.second.plain != str) {
                        std::cout << "\t\tNo matched" << std::endl;
                        return false;
                    }
                }
            }

            JSON::Object dst;
            for(auto& kv : dst_map) {
                dst[kv.first] = kv.second;
            }
            std::stringstream ss;
            ss << dst;
            destination = ss.str();
            std::cout << destination << std::endl;
            return true;
        }

        std::mutex mutex_;
        Commutator::Storage& storage_;
        std::vector<std::shared_ptr<std::thread>> pool_;
        std::list<Commutator::PointPtr> points_;
        std::list<Commutator::PointPtr> ready_points_;
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
                std::shared_ptr<std::thread> thread(new std::thread(&Commutator::Point::Run, p));
                pool_.push_back(thread);
            }
            routes_ = storage_.GetRoutes();
            debugger_ << "Routes" << std::endl;
            for (auto& route : routes_) {
                debugger_ << "\tRoute : " << route.name << "(" << route.route_id << ")" << std::endl
                          << "\tRoute: " << route.source_point_id << "(" << route.source_number_string << ") -> " << route.destination_point_id << "(" << route.destination_number_string << ")" << std::endl;

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
                debugger_ << "\tCHECK point: " << route.source_point_id << " number: " << route.source_number_string << std::endl;
                std::string destination;
                if (route.source_point_id == point->getConfiguration().point_id && checkSourceNumber(route, number, destination)) {
                    debugger_ << "\t\t destination:" << destination << std::endl;
                    std::shared_ptr<Link> link(new Link(route, number, ready_points_));
                    if (link->Connected()) {
                        debugger_ << "\t\tLinked" << std::endl;
                        {
                            std::unique_lock<std::mutex> lock(mutex_);
                            linked_points_.insert(std::make_pair(route.source_point_id, link));
                            linked_points_.insert(std::make_pair(route.destination_point_id, link));
                        }
                        storage_.addCallEvent(route.route_id, number);
                        link->getDestination()->Initiate(destination);
                        link_created = true;
                        break;
                    }
                break;
                }
            }

            if (!link_created) {
                storage_.addCallEvent(0, number);
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

        void OnReady(Commutator::Point* const point)
        {
            debugger_ << "Point " << point->getConfiguration().name << "(" << point->getConfiguration().point_id << ") is ready to work" << std::endl;
            for (auto it = points_.begin(); it != points_.end(); ++it) {
                if (it->get() == point) {
                    storage_.UpdatePointStatus(point->getConfiguration().point_id, Commutator::Storage::Point::Status::psActive);
                    ready_points_.push_back(*it);
                    break;
                }
            }
        }
    };
}