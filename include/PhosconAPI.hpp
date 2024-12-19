#ifndef __LIBPHOSCON_PHOSCONAPI_HPP__
#define __LIBPHOSCON_PHOSCONAPI_HPP__

/*
 * Copyright(C) 2022 RalfO. All rights reserved.
 * https://github.com/RalfOGit/libphoscon
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditionsand the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <string>
#include <vector>
#include <map>
#include <JsonCppWrapper.hpp>
#include <PhosconGW.hpp>

#ifdef LIB_NAMESPACE
namespace LIB_NAMESPACE {
#else
namespace libphoscon {
#endif

    /**
     * Class implementing an API for zigbee devices accessible through a phoscon bridge.
     */
    class PhosconAPI {

    protected:

        static std::string getValueFromJson(const json_value* const json, const std::string& name);
        static bool compareNames(const std::string& name1, const std::string& name2, const bool strict);
        static std::vector<std::string> getPathSegments(const std::string& path);

    public:

        PhosconAPI(void);
        ~PhosconAPI(void) {}

        // Discover phoscon gateway
        std::vector<PhosconGW> discover(void);

        // Api key management
        const std::string unlockApi(const PhosconGW& gw, const std::string & devicetype);

        // Get accessor methods.
        std::string getValueFromPath(const PhosconGW& gw, const std::string& device, const std::string& path) const;   // e.g. "subdevices:1:state:power:value"

        std::vector<std::string> getDevices(const PhosconGW& gw) const;
        std::string getDeviceSummary(const PhosconGW& gw, const std::string& device) const;

        std::map<std::string, JsonCppWrapper::JsonObject> getEntityObjects(const PhosconGW& gw, const std::string& qualifier) const;

        std::map<std::string, JsonCppWrapper::JsonObject> getLights (const PhosconGW& gw) const { return getEntityObjects(gw, "lights");  };
        std::map<std::string, JsonCppWrapper::JsonObject> getSensors(const PhosconGW& gw) const { return getEntityObjects(gw, "sensors"); };
        std::map<std::string, JsonCppWrapper::JsonObject> getGroups (const PhosconGW& gw) const { return getEntityObjects(gw, "groups");  };
        std::map<std::string, JsonCppWrapper::JsonObject> getScenes (const PhosconGW& gw) const { return getEntityObjects(gw, "scenes");  };
        std::map<std::string, JsonCppWrapper::JsonObject> getRules  (const PhosconGW& gw) const { return getEntityObjects(gw, "rules");   };

        // Set accessor methods.
        std::string setValue(const std::string& name, const std::string& value);    // e.g. "Power", can be used if name is well-known and documented

    };

}   // namespace libphoscon

#endif
