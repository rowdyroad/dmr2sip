#pragma once
#include "Point.h"
#include "Storage.h"
#include "SIP/SIP.h"

namespace Commutator {

    class SIPPoint : public Point, public SIPHandler {
        private:
            std::unique_ptr<SIP> sip_;

        public:

            static std::vector<std::string> GetDevicesList()
            {
                return SIP::GetDevicesList();
            }

            SIPPoint(Storage::Point point, PointHandler* const handler)
                : Point(point, handler)
            {
                auto delimiter_pos = point.id.find("/");
                size_t device_index = std::stoi(point.id.substr(0, delimiter_pos));
                std::string id = point.id.substr(delimiter_pos + 1);
                sip_.reset(new SIP(this, device_index));
                sip_->Connect(id, point.password);
            }

            ~SIPPoint()
            {
                sip_->Stop();
            }

            void Run()
            {
                sip_->Run();
            }

            void Stop()
            {
                sip_->Stop();
            }

            void Initiate(const std::string& number)
            {
                sip_->Call(number);
            }

            void Hangup()
            {
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