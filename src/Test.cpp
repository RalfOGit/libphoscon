#if 1
#include <PhosconAPI.hpp>
#include <PhosconGW.hpp>
#include <Logger.hpp>

#ifdef LIB_NAMESPACE
using namespace LIB_NAMESPACE;
#else
using namespace libphoscon;
#endif

static Logger logger("test");


int main(int argc, char** argv) {

    // configure phoscon API
    PhosconAPI api;
    std::vector<PhosconGW> gw = api.discover();
    PhosconGW& gateway = gw[0];

    // check if we already have access, if not acquire an api key
    gateway.setApiKey("609D5F1A34");
    if (gateway.getApiKey() == "") {
        std::string apikey = api.unlockApi(gateway, "Phoscon2InfluxDB");
        logger("apikey : %s\n", apikey.c_str());
    }

    // get api url
    auto url = gateway.getApiUrl();
    logger("ApiUrl : %s\n\n", url.c_str());

    // get list of all zigbee device ids; these are mac addresses
    auto devices = api.getDevices(gateway);

    // iterate through all devices and print what they are
    logger("Devices:\n");
    for (const auto& device : devices) {
        logger("  %s: %s\n", device.c_str(), api.getDeviceSummary(gateway, device).c_str());
    }
    logger("\n");

    // get power consumption from power meter
    auto powermeter1 = api.getValueFromPath(gateway, "70:b3:d5:2b:60:0b:bf:bd", "subdevices:1:state:power:value");
    logger("70:b3:d5:2b:60:0b:bf:bd => subdevices:1:state:power:value : %s\n", powermeter1.c_str());

    auto powermeter2 = api.getValueFromPath(gateway, "70:b3:d5:2b:60:0b:be:0e", "subdevices:1:state:power:value");
    logger("70:b3:d5:2b:60:0b:be:0e => subdevices:1:state:power:value : %s\n", powermeter2.c_str());

    return 0;
}
#endif
