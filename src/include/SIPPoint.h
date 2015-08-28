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
                auto delimiter_pos = point.id.find("/");
                size_t device_index = std::stoi(point.id.substr(0, delimiter_pos));
                std::string id = point.id.substr(delimiter_pos + 1);
                sip_.reset(new SIP(this, device_index));
                sip_->Connect(id, point.password);
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