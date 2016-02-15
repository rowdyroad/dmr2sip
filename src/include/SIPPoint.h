#pragma once
#include "Point.h"
#include "Storage.h"
#include "SIP/SIP.h"

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
                size_t device_index = std::stoi(Storage::getValue(point.configuration, "device_index"));
                std::string id = Storage::getValue(point.configuration, "id");
                std::string password = Storage::getValue(point.configuration, "password");
                std::cout << "Configuration: " << std::endl
                            << "\tDevice index = " << device_index << std::endl
                            << "\tID = " << id << std::endl
                            << "\tPassword = " << password << std::endl;
                sip_.reset(new SIP(this, device_index));
                sip_->Connect(id, password);
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

            void OnCallEnd(SIP* sip)
            {
                Handler()->OnCallEnded(this);
            }

            void OnInCallBegin(SIP* sip)
            {
                Handler()->OnCallReceived(this, sip->CallAddress());
            }
    };

    class SIPPointFactory: public PointFactory {
        public:
            virtual PointPtr Create(Storage::Point point, PointHandler* const handler)  {
                return PointPtr(new SIPPoint(point, handler));
            }
    };
}