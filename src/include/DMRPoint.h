#pragma once
#include "DMR/xnl_connection.h"
#include "StreamDTMFDecoder.h"

#include "Debug.h"
#include "Point.h"
#include "Storage.h"
#include <regex>


namespace Commutator {

    class DMRPoint : public Point, public CXNLConnectionHandler, public StreamDTMFDecoder::Handler {
        private:
            std::unique_ptr<CXNLConnection> connection_;
            std::shared_ptr<Point> remote_point_;
            std::unique_ptr<StreamDTMFDecoder> decoder_;
            Debug debugger_;
        public:
            DMRPoint(const std::string& auth_key, uint32_t delta, Storage::Point point, PointHandler* const handler)
                : Point(point, handler)
                , debugger_("dmrpoint")
            {
                std::string address = Storage::getValue(point.configuration, "address");
                uint16_t port =  std::stoi(Storage::getValue(point.configuration, "port"));
                debugger_ << "Configuration: " << std::endl
                            << "\tAddress = " << address << ":" << port << std::endl;
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
                connection_->SelectChannel(std::stoi(number));
            }

            void Hangup()
            {
                connection_->PTT(PTT_RELEASE);
            }

            void OnChannelSelected(CXNLConnection* connection, uint16_t channel)
            {
                debugger_ << "Channel selected:" << channel << std::endl;
                usleep(100000);

                connection_->PTT(PTT_PUSH);
            }

            void OnCallInitiated(CXNLConnection* connection, const std::string& address)
            {
               Point::Handler()->OnCallReceived(this, address);
            }

            void OnCallEnded(CXNLConnection* connection)
            {
                Point::Handler()->OnCallEnded(this);
            }

            bool Link(PointPtr& point)
            {
                remote_point_ = point;
                decoder_.reset(new StreamDTMFDecoder(this, Storage::getValue(getConfiguration().configuration, "device_index")));
                return true;
            }

            void UnLink(PointPtr& point)
            {
                remote_point_.reset();
                decoder_.reset();
            }

            void OnCode(StreamDTMFDecoder* const sender, uint8_t code)
            {
                debugger_ << "DTMF Code received:" << (char)code << std::endl;
                remote_point_->SendCode(code);
            }
    };

    class DMRPointFactory: public PointFactory {
        private:
            std::string auth_key_;
            uint32_t delta_;
        public:
            DMRPointFactory(const std::string& auth_key, uint32_t delta)
                : auth_key_(auth_key)
                , delta_(delta)
            {}

            virtual PointPtr Create(Storage::Point point, PointHandler* const handler)
            {
                return PointPtr(new DMRPoint(auth_key_, delta_, point, handler));
            }
    };

}