#pragma once
/*#include <sqlite/connection.hpp>
#include <sqlite/result.hpp>
#include <sqlite/query.hpp>
#include <sqlite/execute.hpp>
*/

#include <mysql++/mysql++.h>
#include <memory>
#include <mutex>
#include <regex.h>
#include "Exception.h"
#include "Debug.h"

#include "json.hh"

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

                ~Route()
                {
                    for (auto& kv : source_number) {
                        // regfree(kv.second.regex.get()); todo
                    }
                }

                size_t route_id;
                size_t source_point_id;
                std::map<std::string, SourceMaskedNumberPart> source_number;
                std::string source_number_string;            
                size_t destination_point_id;
                std::string destination_number_string;
                PlaceholderedPhoneNumber destination_number;
                bool phone_mode;
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


                bool checkSourceNumber(const std::string& number, std::string& destination)
                {
                    JSON::Object nmbr = parse_string(number);
                    std::cout << "Source number check: " << nmbr << std::endl;
                    auto dst_map = destination_number;
                    for (auto& kv : source_number) {
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
                auto query = db_->query("SELECT route_id, source_point_id, source_number, destination_point_id, destination_number, phone_mode FROM routes");
                if (auto res = query.store()) {
                    debugger_ << "Routes count: " << res.num_rows() << std::endl;
                    if (res.num_rows() > 0) {
                        for (auto& row: res) {
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
                                for (auto& chunk : (JSON::Array)kv.second) {
                                    try {
                                        size_t id = chunk["id"].as_int();
                                        if (id > 0) {
                                            ss << "(" <<  chunk["value"].as_string() << ")";
                                            has_regexp = true;
                                            source_number_part.placeholders.insert(std::make_pair(index++, id));
                                        } else {
                                            ss << chunk["value"].as_string();
                                        }
                                    } catch (...) 
                                    {
                                        ss << chunk["value"].as_string();
                                    }
                                }

                                if (has_regexp) {
                                    std::string regexp =  ss.str();
                                    debugger_ << "\tRegexp:" << regexp << std::endl;
                                    source_number_part.regex.reset(new regex_t);
                                    if (regcomp(source_number_part.regex.get(), regexp.c_str(), REG_ICASE | REG_EXTENDED)) {
                                        debugger_ <<  debugger_.warn << "Incorrect regexp pattern '" << regexp << "'. Ignoring route rule." << std::endl;
                                        continue;
                                    }                                   
                                } else {
                                    debugger_ << "\tPlain:" << ss.str() << std::endl;
                                    source_number_part.plain = ss.str();
                                }
                                r.source_number.insert(std::make_pair(kv.first, source_number_part));                                
                            }                            
                            r.destination_point_id = row[3];
                            r.destination_number_string = row[4].c_str();
                            auto destination_number = (JSON::Object)parse_string(r.destination_number_string);
                            for (auto& kv : destination_number) {
                                r.destination_number.insert(std::make_pair(kv.first, kv.second.as_string()));
                            }                  
                            r.phone_mode = row[5];
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