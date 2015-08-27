#pragma once
#include "DMR/xnl_connection.h"

#include "Point.h"
#include "Storage.h"


namespace Commutator {

    class DMRPoint : public Point, public CXNLConnectionHandler {
        private:
            std::unique_ptr<CXNLConnection> connection_;
        public:
            DMRPoint(Storage::Point point, PointHandler* const handler)
                : Point(point, handler)
            {
                auto del_pos = point.id.find(":");
                std::string address = point.id.substr(0, del_pos);
                size_t port = std::stoi(point.id.substr(del_pos + 1));

                del_pos = point.password.find(":");

                std::string auth_key = point.password.substr(0, del_pos);
                size_t delta = std::stoul(point.password.substr(del_pos + 1));

                connection_.reset(new CXNLConnection(address, port, auth_key, delta, this));
            }

            void Run()
            {
                connection_->Run();
            }

            void Stop()
            {
                connection_->Stop();
            }

            void Initiate(const std::string& number)
            {
                connection_->select_mic(1);
                connection_->PTT(PTT_PUSH);
            }

            void Hangup()
            {
                connection_->PTT(PTT_RELEASE);
                connection_->select_mic(0);
            }

            void OnCallInitiated(CXNLConnection* connection, const std::string& address)
            {
               Handler()->OnCallReceived(this, address);
            }

            void OnCallEnded(CXNLConnection* connection)
            {
                Handler()->OnCallEnded(this);
            }
    };

    class DMRPointFactory: public PointFactory {
        public:
            virtual PointPtr Create(Storage::Point point, PointHandler* const handler)  {
                return PointPtr(new DMRPoint(point, handler));
            }
    };

}