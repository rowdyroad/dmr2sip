#include <string.h>
#include <string>
#include <stdio.h>
#include <signal.h>
#include <memory>
#include <thread>
#include <map>
#include "Exception.h"
#include "Point.h"
#include "DMRPoint.h"
#include "Debug.h"

#include "json.hh"

using namespace Commutator;

class DMRHandler : public PointHandler {
    public:
        virtual bool OnCallReceived(Point* const point, const std::string& number)
        {
            std::cout << "Call received from " << number << std::endl;
        }
        virtual void OnCallEnded(Point* const point)
        {
            std::cout << "Call ended" << std::endl;
        }
        virtual void OnReady(Point* const point)
        {
            ((DMRPoint*)point)->Initiate("1");
        }
};

int main(int argc, char*argv[])
{
    const char* dmrAuthorizationKey = "0x152C7E9D0x38BE41C70x71E96CA40x6CAC1AFC0x9E3779B9";
    const size_t dmrAuthorizationDelta = 0x9E3779B9;
    Storage::Point config;
    DMRHandler handler;
    config.configuration = "{\"address\":\"192.168.10.1\",\"port\":\"8002\",\"device_index\":\"A\"}";
    JSON::Value v = parse_string("{\"address\":\"192.168.10.1\"}");
    std::cout << v << std::endl;
    DMRPoint dmr(dmrAuthorizationKey, dmrAuthorizationDelta, config, &handler);

    dmr.Run();

    return 0;
}