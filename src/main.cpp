#include <iostream>
#include "xnl_connection.h"


class Connection : public CXNLConnectionHandler
{
	public:
		void OnConnectionSuccess(CXNLConnection* connection)
		{
			std::cout << "Connected" << std::endl;
		}

         void OnConnectionFailure(CXNLConnection* connection)
        {
        	std::cout << "Connection failure" << std::endl;
        }

         void OnXCMPMessage(CXNLConnection* connection, uint8_t* msg)
        {
			std::cout << "Message received" << std::endl;
        }
};

int main()
{
	Connection handler;
	CXNLConnection connection("192.168.10.1", 8002, "", 0, &handler);
	sleep(1000);
	return 0;
}