#pragma once
#include "Point.h"
#include "Storage.h"
#include "SIP/SIP.h"

namespace Commutator {

	class SIPPoint;

	class SIPPointHandler {
		public:
			virtual void OnCallReceived(SIPPoint& point, const std::string& number) = 0;
			virtual void OnCallEnded(SIPPoint& point) = 0;
	};

	class SIPPoint : public Point, public SIPHandler {
		private:
			std::unique_ptr<SIP> sip_;

		public:
			SIPPoint(const Storage::Point& point, PointHandler& handler)
				: Point(point, handler)
			{
				auto delimiter_pos = point.id.find("/");
				size_t device_index = std::stoi(point.id.substr(0, delimiter_pos));
				std::string id = point.id.substr(delimiter_pos);
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
	        	Handler().OnCallEnded(*this);
	        }

			void OnInCallBegin(SIP* sip)
			{
				Handler().OnCallReceived(*this, sip->CallAddress());
			}
	};

	class SIPPointFactory: public PointFactory {
		public:
			Point Create(const Storage::Point& point, PointHandler& handler)  {
				return std::move(SIPPoint(point, handler));
			}
	};
}