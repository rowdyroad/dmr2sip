#include <iostream>
#include "xnl_connection.h"


class Connection : public CXNLConnectionHandler
{
	public:
		void OnConnectionSuccess(CXNLConnection* connection)
		{
			std::cout << "Connected" << std::endl;
			connection->call("001");
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
	CXNLConnection connection("192.168.10.1", 8002, "0x152C7E9D0x38BE41C70x71E96CA40x6CAC1AFC",0x9E3779B9, &handler);
	sleep(1000);
	return 0;
}