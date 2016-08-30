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

#include <json/json.hh>

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
            //((DMRPoint*)point)->Initiate("{\"type\":\"group\", \"number\":\"2077001\"}");
        }
};

int main(int argc, char*argv[])
{
    JSON::Object configuration;
    configuration["address"] = "192.168.10.1";
    configuration["port"] = 8002;
    configuration["device_index"] = "A";
    JSON::Object phone_mode;
    phone_mode["code"] = "##";
    phone_mode["duration"] = 2000;
    configuration["phone_mode"] = phone_mode;



    Storage::Point config;
    config.configuration = configuration;

    DMRHandler handler;
    DMRPoint dmr("0x152C7E9D0x38BE41C70x71E96CA40x6CAC1AFC0x9E3779B9", 0x9E3779B9, config, &handler);
    dmr.Run();

    return 0;
}