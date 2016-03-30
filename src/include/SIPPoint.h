#pragma once
#include "Point.h"
#include "Storage.h"
#include "SIP/SIP.h"
#include "Exception.h"
#include "Debug.h"

namespace Commutator {

    class SIPPoint : public Point, public SIPHandler {
        private:
            std::unique_ptr<SIP> sip_;
            volatile bool quit_;
            std::mutex mutex_;
            Debug debugger_;
        public:

            SIPPoint(Storage::Point point, PointHandler* const handler, const std::string& config)
                : Point(point, handler)
                , quit_(false)
                , debugger_("sippoint")
            {
                std::string username = Storage::getValue(point.configuration, "authorization_username");
                std::string password = Storage::getValue(point.configuration, "password");

                std::string host = Storage::getValue(point.configuration, "host");
                uint16_t port = std::stoi(Storage::getValue(point.configuration, "port"));

                debugger_ << "Configuration: " << std::endl
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
                std::unique_lock<std::mutex> lock(mutex_);
                sip_->Call(number);
            }

            void Hangup()
            {
                std::unique_lock<std::mutex> lock(mutex_);
                sip_->Hangup();
            }

            bool Link(PointPtr& point)
            {
                try {
                    sip_->SelectDevice(Storage::getValue(point->getConfiguration().configuration, "device_index"));
                    return true;
                } catch (Exception& e) {
                    return false;
                }
            }

            void OnCallEnd(SIP* sip)
            {
                Handler()->OnCallEnded(this);
            }

            bool OnInCallBegin(SIP* sip)
            {
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
            virtual PointPtr Create(Storage::Point point, PointHandler* const handler)  {
                return PointPtr(new SIPPoint(point, handler, config_));
            }
    };
}