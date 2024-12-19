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
    if (gateway.getApiKey() == "") {
        api.unlockApi(gateway, "Phoscon2InfluxDB");
    }

    // get api url
    auto url = gateway.getApiUrl();

    // get list of all zigbee device ids; these are mac addresses
    auto devices = api.getDevices(gateway);

    // iterate through all devices and print what they are
    logger("devices:\n");
    for (const auto& device : devices) {
        logger("  %s: %s\n", device.c_str(), api.getDeviceSummary(gateway, device).c_str());
    }

    // get power consumption from power meter
    auto power = api.getValueFromPath(gateway, "70:b3:d5:2b:60:0b:bf:bd", "subdevices:1:state:power:value");

    return 0;
}
#endif
