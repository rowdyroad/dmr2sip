#include <string>
#include <stdio.h>
#include <signal.h>
#include <memory>
#include "SIP.h"
#include "xnl_connection.h"

#include <thread>

volatile bool quit = false;

SIP* sip;
CXNLConnection* radio;



class Handler : public CXNLConnectionHandler, public SIPHandler
{
    private:
        bool incomming_sip_call = false;
    public:

        void OnInCallBegin(SIP* sip)
        {
            printf("INCOMMING CALL FROM %s\n", sip->CallAddress());
            incomming_sip_call = true;
            radio->PTT(PTT_PUSH);
        }


        void OnCallEnd(SIP *sip)
        {
            if (incomming_sip_call) {
                incomming_sip_call = false;
                radio->PTT(PTT_RELEASE);
            }
        }

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
             std::cout << "!!!OnCallInitiated" << std::endl;
            if (!incomming_sip_call)  {
                sip->Call("1100@192.168.0.50:5060");
            }
        }

        void OnCallEnded(CXNLConnection* connection)
        {
            std::cout << "!!!OnCallEnded" << std::endl;
            if (!incomming_sip_call) {
                sip->Hangup();
            }
        }
};


Handler* handler;


void signalHandler( int signum )
{
    sip->Stop();
    radio->Stop();
}

int main(int argc, char*argv[])
{
    signal(SIGINT, signalHandler);

    handler = new Handler;
    sip = new SIP(handler, 2);
    sip->Connect("sip:1001@192.168.0.50:5060", "3be8848500ef9e1b692d7db194d6b4e3");
    radio = new CXNLConnection("192.168.10.1", 8002, "0x152C7E9D0x38BE41C70x71E96CA40x6CAC1AFC",0x9E3779B9, handler);

    std::thread sip_thread = std::thread([=]() { sip->Run(); });
    std::thread connection_thread = std::thread([=]() { radio->Run(); });

    sip_thread.join();
    connection_thread.join();
    delete radio;
    delete sip;
    delete handler;
    return 0;
}