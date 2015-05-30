#pragma once
#include <SQLiteCpp/SQLiteCpp.h>

#include "Exception.h"

namespace Commutator {

    class StorageException : public Exception { };

    class Storage
    {
        private:
            std::unique_ptr<SQLite::Database> db_;

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
                    db.reset(new SQLite::Database(This->db_filename_, SQLITE_OPEN_READWRITE));
                } catch (std::exception& e) {
                    throw new ConfigurationException(3, e.what());
                }
            }

            SQLite::Database& Handle()
            {
                return *db_;
            }

            size_t UpdateAllPointsStatus(size_t status)
            {
                SQLite::Statement  stmt(*db, "UPDATE points SET status  = ?");
                stmt.bind(1, status);
                stmt.exec();
            }

            size_t UpdatePointStatus(size_t point_id, size_t status)
            {
                SQLite::Statement  stmt(*db, "UPDATE points SET status  = ? WHERE point_id = ?");
                stmt.bind(1, status);
                stmt.bind(2, point_id);
                stmt.exec();
            }

            std::vector<Point>&& GetPoints()
            {
                std::vector<Point> points;
                SQLite::Statement   query(*db, "SELECT point_id, type, id, password  FROM points");
                while (query.executeStep()) {
                    Point p;
                    p.point_id = query.getColumn(0);
                    p.type = query.getColumn(1);
                    p.id = query.getColumn(2);
                    p.password = getColumn(3);
                    points.push_back(p);
                }
                return std::move(points);
            }

            std::vector<Route>&& GetRoutes()
            {
                std::vector<Route> routes;
                SQLite::Statement   query(*db, "SELECT route_id, source_point_id, source_number, destination_point_id, destination_number  FROM routes");
                while (query.executeStep()) {
                    Route r;
                    r.route_id = query.getColumn(0);
                    r.source_point_id = query.getColumn(1);
                    r.source_number = query.getColumn(2);
                    r.destination_point_id = getColumn(3);
                    r.destination_number = query.getColumn(2);
                    routes.push_back(r);
                }
                return std::move(routes);
            }

            void addEvent(size_t route_id, const std::string& source_number)
            {
                SQLite::Statement  stmt(*db, "INSERT events (route_id, source_number)  VALUES(?, ?)");
                stmt.bind(1, route_id);
                stmt.bind(2, source_number);
                stmt.exec();
            }
    };

}