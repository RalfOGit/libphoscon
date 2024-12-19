#ifndef __LIBPHOSCON_PHOSCONGW_HPP__
#define __LIBPHOSCON_PHOSCONGW_HPP__

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

#ifdef LIB_NAMESPACE
namespace LIB_NAMESPACE {
#else
namespace libphoscon {
#endif

    /**
     * Class implementing identification data for a phoscon gateway.
     */
    class PhosconGW {

    protected:
        std::string id;
        std::string name;
        std::string internalIpAddress;
        std::string internalPort;
        std::string macAddress;
        std::string publicIpAddress;
        std::string url;
        std::string apiKey;
        std::string apiUrl;

    public:
        PhosconGW(const std::string& _id, const std::string& _name, const std::string& _internalIpAddress,
            const std::string& _internalPort, const std::string& _macAddress, const std::string& _publicIpAddress) :
            id(_id),
            name(_name),
            internalIpAddress(_internalIpAddress),
            internalPort(_internalPort),
            macAddress(_macAddress),
            publicIpAddress(_publicIpAddress) {
            url = "http://" + internalIpAddress + ":" + internalPort + "/api";
            apiUrl = url + "/";
            setApiKey("609D5F1A34");
        }

        const std::string& getId               (void) const { return id; }
        const std::string& getName             (void) const { return name; }
        const std::string& getInternalIpAddress(void) const { return internalIpAddress; }
        const std::string& getInternalPort     (void) const { return internalPort; }
        const std::string& getMacAddress       (void) const { return macAddress; }
        const std::string& getPublicIpAddress  (void) const { return publicIpAddress; }

        const std::string& getUrl              (void) const { return url; }
        const std::string& getApiKey           (void) const { return apiKey; }
        const std::string& getApiUrl           (void) const { return apiUrl; }

        void  setApiKey(const std::string& key) {
            apiKey = key;
            apiUrl = url + "/" + apiKey + "/";
        }

    };

}   // namespace libphoscon

#endif
