#pragma once
#include "DMR/xnl_connection.h"

#include "Point.h"
#include "Storage.h"
#include <regex>


namespace Commutator {

    class DMRPoint : public Point, public CXNLConnectionHandler {
        private:
            std::unique_ptr<CXNLConnection> connection_;
        public:
            DMRPoint(Storage::Point point, PointHandler* const handler)
                : Point(point, handler)
            {
                std::string address = Storage::getValue(point.configuration, "address");
                uint16_t port =  std::stoi(Storage::getValue(point.configuration, "port"));
                std::string auth_key = Storage::getValue(point.configuration, "auth_key");
                uint32_t delta = std::stoul(Storage::getValue(point.configuration, "delta"));
                std::cout << "Configuration: " << std::endl
                            << "\tAddress = " << address << ":" << port << std::endl
                            << "\tAuth key = " << auth_key << std::endl
                            << "\tDelta = " << delta << std::endl;
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