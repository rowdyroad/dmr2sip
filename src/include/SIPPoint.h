#pragma once
#include <string>
#include "Point.h"
#include "Storage.h"
#include "SIP/SIP.h"
#include "Exception.h"
#include "Debug.h"

#include "json.hh"

namespace Commutator {

    class SIPPoint : public Point, public SIPHandler {
        public:
            struct Number {
                std::string number;
                std::string extension;
                Number(const std::string& json)
                {
                    auto value = parse_string(json);
                    auto& nv = value["number"];
                    number = nv.type() == JSON::STRING ? nv.as_string() : std::to_string(nv.as_int());
                    extension = value["extension"].as_string();
                }

                Number(const std::string& number, const std::string& extension)
                    : number(number)
                    , extension(extension)
                {}

                std::string asString() const
                {
                    std::stringstream ss;
                    JSON::Object ret;
                    ret["number"] = number;
                    ret["extension"] = extension;
                    ss << ret;
                    return ss.str();
                }
            };
        private:
            std::unique_ptr<SIP> sip_;
            volatile bool quit_;
            std::mutex mutex_;
            Debug debugger_;
            std::unique_ptr<Number> number_;
        public:

            SIPPoint(const Storage::Point& point, PointHandler* const handler, const std::string& config)
                : Point(point, handler)
                , quit_(false)
                , debugger_("sippoint")
            {
                std::string username = point.configuration["authorization_username"].as_string();
                std::string password = point.configuration["password"].as_string();

                std::string host = point.configuration["host"].as_string();


                auto& pv = point.configuration["port"];
                uint16_t port = pv.type() == JSON::STRING ? std::stoi(pv.as_string()) : pv.as_int();

                debugger_ << "Configuration: " << std::endl
                            << "\tAddress = " << host << std::endl
                            << "\tPort = " << port << std::endl
                            << "\tAuthorization Username = " << username << std::endl
                            << "\tPassword = " << "*********" << std::endl;
                sip_.reset(new SIP(this, config));
                sip_->Connect(host, port, username, password);
            }

            ~SIPPoint()
            {
                Stop();
            }

            void Run()
            {
                   while (!quit_) {
                       {
                        std::unique_lock<std::mutex> lock(mutex_);
                        sip_->Iterate();
                            }
                            usleep(50000);
                        }
                        if (sip_->IsCalling()) {
                            sip_->Hangup();
                        }
            }

            void Stop()
            {
                quit_ = true;
            }

            void Initiate(const std::string& number)
            {
                number_.reset(new Number(number));
                std::unique_lock<std::mutex> lock(mutex_);
                sip_->Call(number_->number);
            }

            void Hangup()
            {
                std::unique_lock<std::mutex> lock(mutex_);
                sip_->Hangup();
            }

            bool Link(PointPtr& point)
            {
                try {
                    sip_->SelectDevice(point->getConfiguration().configuration["device_index"].as_string());
                    return true;
                } catch (Exception& e) {
                    return false;
                }
            }

            void OnCallEnd(SIP* sip)
            {
                number_.reset();
                Handler()->OnCallEnded(this);
            }

            bool PhoneModeMaster()
            {
                return true;
            }

            bool OnInCallBegin(SIP* sip)
            {
                if (number_) {
                    //todo send DTMF
                }
                return Handler()->OnCallReceived(this, sip->CallAddress());
            }

            void SendCode(uint8_t code)
            {
               sip_->SendDTMF(code);
            }
    };

    class SIPPointFactory: public PointFactory {
        private:
            std::string config_;
        public:
            SIPPointFactory(const std::string& config)
                :config_(config)
            {}
            virtual PointPtr Create(const Storage::Point& point, PointHandler* const handler)  {
                return PointPtr(new SIPPoint(point, handler, config_));
            }
    };
}