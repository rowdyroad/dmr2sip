#pragma once
#include "Point.h"
#include "Storage.h"
#include "SIP/SIP.h"
#include "Exception.h"

namespace Commutator {

    class SIPPoint : public Point, public SIPHandler {
        private:
            std::unique_ptr<SIP> sip_;
            volatile bool quit_;
            std::mutex mutex_;

        public:

            static std::vector<std::string> GetDevicesList()
            {
                return SIP::GetDevicesList();
            }

            SIPPoint(Storage::Point point, PointHandler* const handler)
                : Point(point, handler)
		        , quit_(false)
            {
                std::string username = Storage::getValue(point.configuration, "authorization_username");
                std::string password = Storage::getValue(point.configuration, "password");

                std::string host = Storage::getValue(point.configuration, "host");
                uint16_t port = std::stoi(Storage::getValue(point.configuration, "port"));

                std::cout << "Configuration: " << std::endl
                            << "\tAuthorization Username = " << username << std::endl
                            << "\tPassword = " << "*********" << std::endl;
                sip_.reset(new SIP(this));
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
                    size_t device_index = std::stoi(Storage::getValue(point->getConfiguration().configuration, "device_index"));
                    sip_->SelectDevice(device_index);
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
    };

    class SIPPointFactory: public PointFactory {
        public:
            virtual PointPtr Create(Storage::Point point, PointHandler* const handler)  {
                return PointPtr(new SIPPoint(point, handler));
            }
    };
}