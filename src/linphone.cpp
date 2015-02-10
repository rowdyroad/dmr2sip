#include <string>
#include <stdio.h>
#include <signal.h>
#include <memory>
#include "SIP.h"
#include "xnl_connection.h"

#include <thread>

#include <libconfig.h++>


using namespace libconfig;
volatile bool quit = false;

SIP* sip;
CXNLConnection* radio;

struct Configuration
{
    std::string sip_id;
    std::string sip_password;
    std::string sip_call_by_ptt;
    size_t device;

    std::string radio_address;
    uint32_t radio_port;
    uint64_t radio_delta;
    std::string radio_auth_key;
};


Configuration config;


class Handler : public CXNLConnectionHandler, public SIPHandler
{
    private:
        bool incomming_sip_call = false;
    public:

        void OnInCallBegin(SIP* sip)
        {
            printf("### Incomming call from sip %s\n", sip->CallAddress());
            incomming_sip_call = true;
            radio->select_mic(1);
            radio->PTT(PTT_PUSH);
        }


        void OnCallEnd(SIP *sip)
        {
            if (incomming_sip_call) {
                incomming_sip_call = false;
                radio->PTT(PTT_RELEASE);
                radio->select_mic(0);
            }
        }

        void OnConnectionSuccess(CXNLConnection* connection)
        {
            std::cout << "### Radio Connected" << std::endl;
        }

         void OnConnectionFailure(CXNLConnection* connection)
        {
            std::cout << "### Radio Connection failure" << std::endl;
        }

         void OnXCMPMessage(CXNLConnection* connection, uint8_t* msg)
        {
            std::cout << "### Radio Message received" << std::endl;
        }

        void OnCallInitiated(CXNLConnection* connection, const std::string& address)
        {
             std::cout << "### Radio Call Initiated" << incomming_sip_call <<  std::endl;
            if (!incomming_sip_call)  {
                sip->Call(config.sip_call_by_ptt);
            }
        }

        void OnCallEnded(CXNLConnection* connection)
        {
            std::cout << "### Radio Call Ended" << incomming_sip_call <<  std::endl;
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

    if (argc > 1) {
    	if (!strcmp(argv[1], "-d")) {
    	    auto devices = SIP::GetDevicesList();
    	    std::cout << "Devices list:" << std::endl;
    	    for (auto& p : devices) {
    		  std::cout << "\t" << p << std::endl;
    	    }
    	    return 0;
    	}
    }

    Config cfg;
    try
    {
	cfg.readFile("sip2dmr.cfg");

	cfg.lookupValue("sip.id", config.sip_id);
	cfg.lookupValue("sip.password", config.sip_password);
	cfg.lookupValue("sip.call_by_ptt",config.sip_call_by_ptt);
	cfg.lookupValue("system.device", config.device);
	cfg.lookupValue("radio.address", config.radio_address);;
	cfg.lookupValue("radio.port", config.radio_port);
	cfg.lookupValue("radio.auth_key", config.radio_auth_key);
	cfg.lookupValue("radio.delta", config.radio_delta);
    }
    catch(const FileIOException &fioex)
    {
        std::cerr << "I/O error while reading file." << std::endl;
	return(EXIT_FAILURE);
     }
    catch(const ParseException &pex)
     {
	std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
              << " - " << pex.getError() << std::endl;
	return(EXIT_FAILURE);
    }

    signal(SIGINT, signalHandler);

    handler = new Handler;
    sip = new SIP(handler, 2);
    sip->Connect(config.sip_id, config.sip_password);
    radio = new CXNLConnection(config.radio_address, config.radio_port, config.radio_auth_key, config.radio_delta, handler);
    std::thread sip_thread = std::thread([=]() { sip->Run(); });
    std::thread connection_thread = std::thread([=]() { radio->Run(); });

    sip_thread.join();
    connection_thread.join();
    delete radio;
    delete sip;
    delete handler;
    return 0;
}	