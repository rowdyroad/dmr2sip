#pragma once

#include "Point.h"
#include "Storage.h"


namespace Commutator {


	class DMRPoint;


	class DMRPoint : public Point,  public CXNLConnectionHandler {
		private:
			std::unqie_ptr<CXNLConnection> connection_;


		public:
			DMRPoint(const Storage::Point& point, PointHandler& handler)
				: Point(point, handler)
			{
				auto del_pos = point.id.find(":");
				std::string address = point.id.substr(0, del_pos);
				size_t port = std::stoi(point.id.substr(del_pos));

				del_pos = point.password.find(":");

				std::string auth_key = point.password.substr(0, del_pos);
				std::string delta = point.password.substr(del_pos);

				connection_.reset(new CXNLConnection(address, port, auth_key, delta, this));
			}

			void Run()
			{
				connection_->run();
			}

			void Initiate(const std::string& number)
			{
				connection_->PTT(PTT_PUSH);
                connection_->select_mic(0);
			}

			void Hangup()
			{
  				connection_->PTT(PTT_RELEASE);
                connection_->select_mic(0);
			}

			void OnCallInitiated(CXNLConnection* connection, const std::string& address)
			{
				Handler().OnCallReceived(*this, address);
			}

	    	void OnCallEnded(CXNLConnection* connection)
	    	{
	    		Handler().OnCallEnded(*this);
	    	}
	};

	class DMRPointFactory: public PointFactory {
		public:
			Point Create(const Storage::Point& point, PointHandler& handler)  {
				return std::move(DMRPoint(point, handler));
			}
	};

}