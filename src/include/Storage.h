#pragma once
/*#include <sqlite/connection.hpp>
#include <sqlite/result.hpp>
#include <sqlite/query.hpp>
#include <sqlite/execute.hpp>
*/

#include <mysql++/mysql++.h>
#include <string>
#include <memory>
#include <mutex>
#include <regex.h>
#include "Exception.h"
#include "Debug.h"

#include "json.hh"

int JSONAlwaysInt(const JSON::Value& value)
{
    switch (value.type())
    {
        case JSON::STRING:
            return std::stoi(value.as_string());
        case JSON::INT:
            return value.as_int();
        case JSON::FLOAT:
            return (int)value.as_float();
        case JSON::BOOL:
            return (int)value.as_bool();
    }
    return 0;
}

std::string JSONAlwaysString(const JSON::Value& value)
{
    switch (value.type())
    {
        case JSON::STRING:
            return value.as_string();
        case JSON::INT:
            return std::to_string(value.as_int());
        case JSON::FLOAT:
            return std::to_string(value.as_float());
        case JSON::BOOL:
            return std::to_string(value.as_bool());
        case JSON::NIL:
            return std::string();
    }
    std::stringstream ss;
    ss << value;
    return ss.str();
}

namespace Commutator {

    class StorageException : public Exception { };

    typedef JSON::Value PointConfiguration;
    typedef JSON::Object MaskedPhoneNumber;
    typedef std::map<std::string, std::string> PlaceholderedPhoneNumber;

    class Storage
    {
        private:
            std::unique_ptr<mysqlpp::Connection> db_;
            std::mutex mutex_;
            Debug debugger_;
        public:
            struct Point {
                enum Status {
                    psInvactive = 0,
                    psActive
                };
                size_t point_id;
                std::string type;
                std::string name;
                PointConfiguration configuration;
            };

            struct Route {
                struct SourceMaskedNumberPart
                {
                    std::string plain;
                    std::shared_ptr<regex_t> regex;
                    std::map<size_t, size_t> placeholders;
                };

                struct DestinationPlaceholder
                {
                    std::string text;
                    size_t placeholder;
                };

                ~Route()
                {
                    for (auto& kv : source_number) {
                        // regfree(kv.second.regex.get()); todo
                    }
                }
                std::string name;
                size_t route_id;
                size_t source_point_id;
                std::map<std::string, SourceMaskedNumberPart> source_number;
                std::string source_number_string;
                size_t destination_point_id;
                std::string destination_number_string;
                std::map<std::string, std::vector<DestinationPlaceholder>> destination_number;
                bool phone_mode;
            };

            Storage(const std::string& host, const std::string& database, const std::string& username, const std::string& password, uint16_t port = 3306)
                : debugger_("storage")
            {
                try {
                    db_.reset(new mysqlpp::Connection());
                    db_->connect(database.c_str(), host.c_str(), username.c_str(), password.c_str(), port);
                } catch (std::exception& e) {
                    throw new Exception(3, e.what());
                }
            }

            mysqlpp::Connection& Handle()
            {
                return *db_;
            }

            size_t UpdateAllPointsStatus(size_t status)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                auto query = db_->query();
                query << "UPDATE points SET status = " << (int)status;
                query.execute();
            }

            size_t UpdatePointStatus(size_t point_id, size_t status)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                auto query = db_->query();
                query << "UPDATE points SET status  = " << (int)status << " WHERE point_id = " << (int)point_id;
                query.execute();
            }

            std::vector<Point> GetPoints()
            {
                std::vector<Point> points;
                auto query = db_->query("SELECT point_id, type, name, configuration FROM points");
                if (auto res = query.store()) {
                    if (res.num_rows() > 0) {
                        for (auto& row : res) {
                            Point p;
                            p.point_id = row[0];
                            p.type = row[1].c_str();
                            p.name = row[2].c_str();
                            p.configuration = parse_string(row[3].c_str());
                            points.push_back(p);
                        }
                    }
                }
                return std::move(points);
            }

            std::vector<Route> GetRoutes()
            {
                debugger_ << "Getting routes" << std::endl;
                std::vector<Route> routes;
                auto query = db_->query("SELECT route_id, source_point_id, source_number, destination_point_id, destination_number, phone_mode, name FROM routes ORDER BY `order`");
                if (auto res = query.store()) {
                    debugger_ << "Routes count: " << res.num_rows() << std::endl;
                    if (res.num_rows() > 0) {
                        for (auto& row: res) {
                            debugger_ << "Route " << row[6].c_str() << std::endl;
                            Route r;
                            r.route_id = row[0];
                            r.source_point_id = row[1];
                            debugger_ << row[2].c_str() << std::endl;
                            r.source_number_string = row[2].c_str();
                            auto source_number = (JSON::Object)parse_string(r.source_number_string);
                            size_t index = 1;
                            for (auto& kv : source_number) {
                                Route::SourceMaskedNumberPart source_number_part;
                                std::stringstream ss;
                                bool has_regexp = false;
                                if (kv.second.type() == JSON::ARRAY) {
                                    for (auto& chunk : (JSON::Array)kv.second) {
                                       if (chunk["id"].type() == JSON::NIL) {
                                            ss << JSONAlwaysString(chunk["value"]);
                                       } else {
                                            int id = JSONAlwaysInt(chunk["id"]);
                                            ss << "(" <<  JSONAlwaysString(chunk["value"]) << ")";
                                            has_regexp = true;
                                            source_number_part.placeholders.insert(std::make_pair(index++, id));
                                       }
                                    }
                                } else {
                                    ss << JSONAlwaysString(kv.second);
                                }

                                debugger_ << kv.first;

                                if (has_regexp) {
                                    std::string regexp =  ss.str();
                                    debugger_ << " - Regexp:" << regexp << std::endl;
                                    source_number_part.regex.reset(new regex_t);
                                    if (regcomp(source_number_part.regex.get(), regexp.c_str(), REG_ICASE | REG_EXTENDED)) {
                                        debugger_ <<  debugger_.warn << "Incorrect regexp pattern '" << regexp << "'. Ignoring route rule." << std::endl;
                                        continue;
                                    }
                                } else {
                                    debugger_ << " - Plain:" << ss.str() << std::endl;
                                    source_number_part.plain = ss.str();
                                }
                                r.source_number.insert(std::make_pair(kv.first, source_number_part));
                            }
                            r.destination_point_id = row[3];
                            r.destination_number_string = row[4].c_str();
                            auto destination_number = (JSON::Object)parse_string(r.destination_number_string);
                            for (auto& kv : destination_number) {
                                if (kv.second.type() == JSON::ARRAY) {
                                    std::vector<Route::DestinationPlaceholder> placeholders;
                                    for (auto& ph : (JSON::Array)kv.second) {
                                        if (ph["id"].type() == JSON::NIL) {
                                            placeholders.push_back(Route::DestinationPlaceholder({JSONAlwaysString(ph["value"]), 0}));
                                        } else {
                                            placeholders.push_back(Route::DestinationPlaceholder({std::string(), (size_t)JSONAlwaysInt(ph["id"])}));
                                        }
                                    }
                                    r.destination_number.insert(std::make_pair(kv.first, placeholders));
                                } else {
                                    r.destination_number.insert(std::make_pair(kv.first, std::vector<Route::DestinationPlaceholder>({Route::DestinationPlaceholder({JSONAlwaysString(kv.second), 0})})));
                                }
                            }
                            r.phone_mode = row[5];
                            r.name = row[6].c_str();
                            routes.push_back(r);
                        }
                        debugger_ << "Done getting routes" << std::endl;
                    }
                }
                return std::move(routes);
            }

            void addCallEvent(size_t route_id, const std::string& source_number)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                auto query = db_->query("INSERT INTO events (route_id, source_number,type)  VALUES(");
                query << (int)route_id << ", '" << source_number << "','call')";
                query.execute();
            }
    };
}