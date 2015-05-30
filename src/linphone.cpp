#include <string>
#include <stdio.h>
#include <signal.h>
#include <memory>
#include "SIP.h"
#include "xnl_connection.h"

#include <thread>


using namespace libconfig;
volatile bool quit = false;

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


void signalRestartHandler(int signum)
{

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

    signal(SIGINT, signalHandler);
    signal(SIGHUP, signalRestartHandler);

    std::vector<std::thread> pool;
    Commutator:::Configuration config("sip2dmr.conf");
    Commutator::Storage storage(config.getDbFilename());
    storage.UpdateAllPointsStatus(Storage::Point::Status::psInvactive);


    std::map<std::string, std::unique_ptr<Commutator::PointFactory>> factories = {
        {"sip", std::shared_ptr<Commutator::PointFactory>(new Commutator::SIPFactory())},
        {"dmr", std::shared_ptr<Commutator::PointFactory>(new Commutator::DMRFactory())}
    };

    std::vector<Point> points;

    for (auto& point : storage.GetPoints()) {
        Point p = factories[point.type]->Create();
        points.push_back(std::move(p));
        pool.push_back(std::thread([=]{
            p.Run();
        }));
    }



    for (auto& route: storage.GetRoutes()) {

    }

    for (auto& thread: pool) {
        thread.join();
    }

    return 0;
}