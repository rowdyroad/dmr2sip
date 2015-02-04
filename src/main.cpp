#include <iostream>
#include <stdlib.h>
#include "xnl_connection.h"
#include <sys/types.h>
#include <signal.h>


class Connection : public CXNLConnectionHandler
{
    private:
	pid_t linphone;

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

	void OnCallInitiated(CXNLConnection* connection, const std::string& address)
	{
	    std::cout << "Call initiated from " << address << std::endl;
	    pid_t i = fork();
	    if (i == 0) {
		 execlp("./linphone", "./linphone", "1100@192.168.0.50:5060", NULL);
	    } else {
		linphone = i;
	    }
	}


	void OnCallEnded(CXNLConnection* connection)
	{
		std::cout << "Call ended" << std::endl;

		kill(linphone, SIGINT);
	}
};

int main()
{
	Connection handler;
	CXNLConnection connection("192.168.10.1", 8002, "0x152C7E9D0x38BE41C70x71E96CA40x6CAC1AFC",0x9E3779B9, &handler);
	connection.run();
	return 0;
}