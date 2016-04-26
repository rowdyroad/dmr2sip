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

            std::unique_ptr<DestinationNumber> number_;
        public:
            DMRPoint(const std::string& auth_key, uint32_t delta, const Storage::Point& point, PointHandler* const handler)
                : Point(point, handler)
                , debugger_("dmrpoint")
            {
                std::string address = point.configuration["address"].as_string();
                auto& pv = point.configuration["port"];
                uint16_t port = pv.type() == JSON::STRING ? std::stoi(pv.as_string()) : pv.as_int();
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

            void Initiate(const DestinationNumber& number)
            {
                number_.reset(new DestinationNumber(number));

                const std::string& type = number["type"].as_string();

                auto& nv = number["number"];

                int destination_number = nv.type() == JSON::STRING ? std::stoi(nv.as_string()) : nv.as_int();

                if (type == "private") {
                    connection_->send_xcmp_call_ctrl_request(0x01, 0x04, 0x01, destination_number, 0);
                } else if (type == "group") {
                    connection_->send_xcmp_call_ctrl_request(0x01, 0x06, 0x01, 0, destination_number);
                } else if (type == "channel") {
                    connection_->SelectChannel(destination_number);
                }
                // usleep(200000);
                // connection_->PTT(PTT_PUSH);
                // number_ = number;
            }

            void Hangup()
            {
                connection_->PTT(PTT_RELEASE);
            }

            void OnConnectionSuccess(CXNLConnection* connection)
            {
                debugger_ << "Device is ready" << std::endl;
                Point::Handler()->OnReady(this);
            }

            void OnChannelSelected(CXNLConnection* connection, uint16_t channel)
            {
                debugger_ << "Channel selected:" << channel << std::endl;
                usleep(100000);
                connection_->PTT(PTT_PUSH);
            }

            void OnCallInitiated(CXNLConnection* connection, const std::string& address)
            {
                if (number_) {
                    try {
                        std::string extension = (*number_)["extension"].as_string();

                    } catch(std::logic_error& e) { }
                }
               Point::Handler()->OnCallReceived(this, address);
            }

            void OnCallEnded(CXNLConnection* connection)
            {
                number_.reset();
                Point::Handler()->OnCallEnded(this);
            }

            bool Link(PointPtr& point)
            {
                remote_point_ = point;
                decoder_.reset(new StreamDTMFDecoder(this, getConfiguration().configuration["device_index"].as_string()));
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

            virtual PointPtr Create(const Storage::Point& point, PointHandler* const handler)
            {
                return PointPtr(new DMRPoint(auth_key_, delta_, point, handler));
            }
    };

}