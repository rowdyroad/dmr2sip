#pragma once
//#include <SQLiteCpp/SQLiteCpp.h>
#include <sqlite/connection.hpp>
#include <sqlite/result.hpp>
#include <sqlite/query.hpp>
#include <sqlite/execute.hpp>

#include "Exception.h"

namespace Commutator {

    class StorageException : public Exception { };

    class Storage
    {
        private:
            std::unique_ptr<sqlite::connection> db_;

        public:
            struct Point {
                enum Status {
                    psInvactive = 0,
                    psActive
                };
                size_t point_id;
                std::string type;
                std::string id;
                std::string password;
            };

            struct Route {
                size_t route_id;
                size_t source_point_id;
                std::string source_number;
                size_t destination_point_id;
                std::string destination_number;
            };

            Storage(const std::string& filename)
            {
                try {
                    db_.reset(new sqlite::connection(filename));
                } catch (std::exception& e) {
                    throw new ConfigurationException(3, e.what());
                }
            }

            sqlite::connection& Handle()
            {
                return *db_;
            }

            size_t UpdateAllPointsStatus(size_t status)
            {
        sqlite::execute command(*db_, "UPDATE points SET status  = ?");
                command.bind(1, (int)status);
                command.emit();
            }

            size_t UpdatePointStatus(size_t point_id, size_t status)
            {
                sqlite::execute stmt(*db_, "UPDATE points SET status  = ? WHERE point_id = ?");
                stmt.bind(1, (int)status);
                stmt.bind(2, (int)point_id);
                stmt.emit();
            }

            std::vector<Point> GetPoints()
            {
                std::vector<Point> points;
                sqlite::query query(*db_, "SELECT point_id, type, id, password  FROM points");

        auto result = query.emit_result();
        if (result->get_row_count() > 0) {
                    do {
                    Point p;
                    p.point_id = result->get_int(0);
                    p.type = result->get_string(1);
                        p.id = result->get_string(2);
                        p.password = result->get_string(3);
                    points.push_back(p);
                    } while (result->next_row());
        }
                return std::move(points);
            }

            std::vector<Route> GetRoutes()
            {
                std::vector<Route> routes;
                sqlite::query query(*db_, "SELECT route_id, source_point_id, source_number, destination_point_id, destination_number  FROM routes");
                auto result = query.emit_result();

        if (result->get_row_count() > 0) {
            do {
                    Route r;
                    r.route_id = result->get_int(0);
                    r.source_point_id = result->get_int(1);
                    r.source_number = result->get_string(2);
                    r.destination_point_id = result->get_int(3);
                    r.destination_number = result->get_string(4);
                    routes.push_back(r);
            } while (result->next_row());
                }
                return std::move(routes);
            }

            void addEvent(size_t route_id, const std::string& source_number)
            {
                sqlite::execute  stmt(*db_, "INSERT events (route_id, source_number)  VALUES(?, ?)");
                stmt.bind(1, (int)route_id);
                stmt.bind(2, source_number);
                stmt.emit();
            }
    };

}