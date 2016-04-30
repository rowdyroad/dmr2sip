#pragma once
#include <string>
#include "DMR/xnl_connection.h"
#include "StreamDTMFDecoder.h"

#include "Debug.h"
#include "Point.h"
#include "Storage.h"
#include <regex>
#include <chrono>

#include "json.hh"

namespace Commutator {

    class DMRPoint : public Point, public CXNLConnectionHandler, public StreamDTMFDecoder::Handler {
        public:
            struct Number
            {
                enum Type {
                    PRIVATE,
                    GROUP,
                    CHANNEL
                };
                Type type;
                size_t number;
                std::string extension;

                Number(const std::string& json)
                {
                    auto value = parse_string(json);
                    auto tv = value["type"].as_string();
                    if (tv == "private") {
                        type = PRIVATE;
                    } else if (tv == "group") {
                        type = GROUP;
                    } else if (tv == "channel") {
                        type = CHANNEL;
                    } else {
                        throw new std::logic_error("Incorrect destination number type");
                    }

                    auto& nv = value["number"];
                    number = nv.type() == JSON::STRING ? std::stoi(nv.as_string()) : nv.as_int();
                    extension = value["extension"].as_string();
                }

                Number(Type type, size_t number, const std::string& extension)
                    : type(type)
                    , number(number)
                    , extension(extension)
                {}

                Number(Type type, size_t number)
                    : type(type)
                    , number(number)
                {}

                std::string asString() const
                {
                    std::stringstream ss;
                    JSON::Object ret;
                    switch(type) {
                        case PRIVATE:
                            ret["type"] = "private";
                        break;
                        case GROUP:
                            ret["type"] = "group";
                        break;
                        case CHANNEL:
                            ret["type"] = "channel";
                        break;
                    };
                    ret["number"] = std::to_string(number);
                    ret["extension"] = extension;
                    ss << ret;
                    return ss.str();
                }
            };

        private:
            std::unique_ptr<CXNLConnection> connection_;
            std::shared_ptr<Point> remote_point_;
            std::unique_ptr<StreamDTMFDecoder> decoder_;
            Debug debugger_;

            std::unique_ptr<Number> destination_number_;
            std::unique_ptr<Number> source_number_;
            class DTMFCodeState
            {
                size_t first_time_ = 0;
                size_t last_time_;
                size_t duration_;
                std::string wait_code_;
                std::string code_;
                Debug debugger_;
                size_t getCurrentTime() const
                {
                    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
                }

                public:
                    DTMFCodeState(size_t duration, const std::string& code)
                        : duration_(duration)
                        , wait_code_(code)
                        , debugger_("DTMFCodeState")
                    { }

                    void operator+=(uint8_t ch)
                    {
                        if (!first_time_ || getCurrentTime() - first_time_ > duration_) {
                            first_time_ = getCurrentTime();
                            code_ = ch;
                        } else {
                            last_time_ = getCurrentTime();
                            code_ += ch;
                        }

                        if (code_ == wait_code_) {
                            debugger_ << "Turning on phone mode master" << std::endl;
                        }
                    }

                    operator bool() const
                    {
                        return code_ == wait_code_;
                    }
            };

            std::unique_ptr<DTMFCodeState> dmtf_state_;
        public:
            DMRPoint(const std::string& auth_key, uint32_t delta, const Storage::Point& point, PointHandler* const handler)
                : Point(point, handler)
                , debugger_("dmrpoint")
            {
                try {
                    auto& dv = point.configuration["phone_mode"]["duration"];
                    auto& cd = point.configuration["phone_mode"]["code"];
                    dmtf_state_.reset( new DTMFCodeState(dv.type() == JSON::STRING ? std::stoi(dv.as_string()) : dv.as_int(), cd.as_string()));
                } catch (std::logic_error& e) {
                    debugger_ << "Phone mode is disabled: incorrect configuration" << std::endl;
                }

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

            void Initiate(const std::string& number)
            {
                source_number_.reset();
                destination_number_.reset(new Number(number));

                debugger_ << "Call to " << destination_number_->asString() << std::endl;
                switch(destination_number_->type) {
                    case Number::PRIVATE:
                        connection_->send_xcmp_call_ctrl_request(0x01, 0x04, 0x01, destination_number_->number, 0);
                    break;
                    case Number::GROUP:
                        connection_->send_xcmp_call_ctrl_request(0x01, 0x06, 0x01, 0, destination_number_->number);
                    break;
                    case Number::CHANNEL:
                        connection_->SelectChannel(destination_number_->number);
                    break;
                }
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
                debugger_ << "Channel selected: channel=" << channel << std::endl;
                usleep(100000);
                connection_->PTT(PTT_PUSH);
            }

            void OnCallReady(CXNLConnection* connection)
            {
                debugger_ << "OnCallReady" << std::endl;
                connection_->PTT(PTT_PUSH);
                //todo tone DTMF number
            }

            void OnCallEstablished(CXNLConnection* connection, uint8_t call_type, const std::string& number, const std::string& group)
            {
                debugger_ << "OnCallEstablished: type=" << (size_t)call_type << " number=" << number << " group=" << group << std::endl;

                if (!destination_number_) {
                    Number::Type type;
                    std::string nmbr;
                    switch (call_type) {
                        case 0x04:
                            type = Number::PRIVATE;
                            nmbr = number;
                        break;
                        case 0x06:
                            type = Number::GROUP;
                            nmbr = group;
                        break;
                    };
                    source_number_.reset(new Number(type, std::stoi(nmbr)));
                    Point::Handler()->OnCallReceived(this, group + ":" + number);
                }
                decoder_.reset(new StreamDTMFDecoder(this, getConfiguration().configuration["device_index"].as_string()));
            }

            void Callback()
            {
                if (source_number_) {
                    debugger_ << "Callback to source number " << source_number_->asString() << std::endl;
                    Initiate(source_number_->asString());
                }
            }

            void OnCallEnded(CXNLConnection* connection)
            {
                connection_->PTT(PTT_RELEASE);
                destination_number_.reset();
                Point::Handler()->OnCallEnded(this);
            }

            bool Link(PointPtr& point)
            {
                remote_point_ = point;
                return true;
            }

            void UnLink(PointPtr& point)
            {
                remote_point_.reset();
                decoder_.reset();
            }

            bool PhoneModeMaster()
            {
                return dmtf_state_ && *dmtf_state_;
            }

            void OnCode(StreamDTMFDecoder* const sender, uint8_t code)
            {
                debugger_ << "DTMF Code received:" << (char)code << std::endl;
                if (dmtf_state_ ) {
                    (*dmtf_state_)+=code;
                }
                if (remote_point_) {
                    remote_point_->SendCode(code);
                }
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