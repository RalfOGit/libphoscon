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
 *    notice, this list of conditions and the following disclaimer in the
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
#define _CRT_SECURE_NO_WARNINGS

#include <PhosconAPI.hpp>
#include <HttpClient.hpp>
#include <Url.hpp>
#include <JsonCppWrapper.hpp>
#include <Logger.hpp>
#include <locale>

#ifdef LIB_NAMESPACE
using namespace LIB_NAMESPACE;
#else
using namespace libphoscon;
#endif

static Logger logger("PhosconAPI");

/**
 * Constructor.
 */
PhosconAPI::PhosconAPI(void) {}

/**
 * Discover phoscon gateway(s) on local area network
 * @return an array of PhosconGW objects
 */
std::vector<PhosconGW> PhosconAPI::discover(void) {
    std::vector<PhosconGW> result;

    // send http discover request
    std::string response, content;
    int http_return_code = HttpClient().sendHttpGetRequest("http://phoscon.de/discover", response, content);

    // check if the http return code is 200 OK
    if (http_return_code == 200) {
        // parse json content
        json_value* json = json_parse(content.c_str(), content.length());

        // traverse through json tree; expected is an array of gateways with properties for each gateway
        logger("discover:\n");
        for (const auto gateway : JsonCppWrapper::JsonArray(json)) {
            if (gateway.getType() == json_object) {
                const auto gw = gateway.asObject();
                for (const auto property : gw) {
                    logger("  %s: \"%s\"\n", property.getName().c_str(), property.getValueAsString().c_str());
                }
                logger("\n");

                std::string id                = gw["id"].getValueAsString();
                std::string name              = gw["name"].getValueAsString();
                std::string internalipaddress = gw["internalipaddress"].getValueAsString();
                std::string internalport      = gw["internalport"].getValueAsString();
                std::string macaddress        = gw["macaddress"].getValueAsString();
                std::string publicipaddress   = gw["publicipaddress"].getValueAsString();
                result.push_back(PhosconGW(id, name, internalipaddress, internalport, macaddress, publicipaddress));
            }
        }
        json_value_free(json);
    }
    return result;
}


/**
 * Unlock the phoscon gateway
 */
const std::string PhosconAPI::unlockApi(const PhosconGW& gw, const std::string& devicetype) {

    // send http post api request
    HttpClient http_client;
    std::string request_data = "{ \"devicetype\": \"" + devicetype + "\" }";
    std::string response, content;
    int http_return_code = HttpClient().sendHttpPostRequest(gw.getUrl(), request_data, response, content);

    if (http_return_code == 403 || http_return_code == 200) {
        // parse json content
        json_value* json = json_parse(content.c_str(), content.length());

        // traverse json tree; expected is an array with one element "success" or "error" 
        if (json != NULL && json->type == json_array) {
            _json_value** array_values = json->u.array.values;
            if (array_values != NULL) {
                unsigned int array_length = json->u.array.length;
                for (unsigned int i = 0; i < array_length; ++i) {
                    JsonCppWrapper::JsonNamedValueVector level0_values = JsonCppWrapper::getNamedValues(array_values[i]);
                    if (level0_values.size() > 0 && level0_values[0].getType() == json_object) {
                        std::string result = level0_values[0].getName();
                        JsonCppWrapper::JsonObject object = level0_values[0].asObject();
                        const json_object_entry* elements = object.getElements();
                        unsigned int         num_elements = object.getNumElements();
                        JsonCppWrapper::JsonNamedValueVector level1_values = JsonCppWrapper::getNamedValues(elements, num_elements);
                        logger("unlockApi: %s\n", result.c_str());
                        for (const auto& value : level1_values) {
                            logger("  %s: \"%s\"\n", value.getName().c_str(), value.getString().getValue().c_str());
                        }
                        if (result == "error") {
                            JsonCppWrapper::JsonNamedValue description = JsonCppWrapper::getValue(elements, num_elements, "description", NULL);
                            if (description.getType() == json_string) {
                                return description.getString().getValue();
                            }
                        }
                        if (result == "success") {
                            JsonCppWrapper::JsonNamedValue username = JsonCppWrapper::getValue(elements, num_elements, "username", NULL);
                            if (username.getType() == json_string) {
                                return username.getString().getValue();
                                //username: "3F99BC34D3"
                                //username: "609D5F1A34"
                            }
                        }
                        //JsonCppWrapper::JsonNamedValueVector level1_values = JsonCppWrapper::getNamedValues(elements, num_elements);
                        return "unlockApi: unexpected result " + result;
                    }
                }
            }
        }
        json_value_free(json);
    }
    return "";
}


/**
 * Get a list of all zigbee devices connected to the gateway.
 * @param gw phoscon gateway
 * @return a vector of zigbee device ids
 */
std::vector<std::string> PhosconAPI::getDevices(const PhosconGW& gw) const {
    std::vector<std::string> devices;

    // send http get api request
    std::string response, content;
    int http_return_code = HttpClient().sendHttpGetRequest(gw.getApiUrl() + "devices", response, content);

    if (http_return_code == 200) {
        // parse json content
        json_value* json = json_parse(content.c_str(), content.length());

        // traverse json tree; expected is an array with one string element for each zigbee entity
        for (const auto id : JsonCppWrapper::JsonArray(json)) {
            if (id.getType() == json_string) {
                std::string str = id.asString().getValueAsString();
                devices.push_back(str);
            }
        }
        json_value_free(json);
    }
    return devices;
}


/**
 * Get a summary list of all zigbee devices connected to the gateway.
 * @param gw phoscon gateway
 * @param device device identifier
 * @return a summary string
 */
std::string PhosconAPI::getDeviceSummary(const PhosconGW& gw, const std::string& deviceid) const {
    std::string summary;

    // send http get api request
    std::string response, content;
    int http_return_code = HttpClient().sendHttpGetRequest(gw.getApiUrl() + "devices/" + deviceid, response, content);

    if (http_return_code == 200) {
        // parse json content
        json_value* json = json_parse(content.c_str(), content.length());

        // traverse json tree; expected is an object with device and subdevice properties
        if (json != NULL && json->type == json_object) {
            JsonCppWrapper::JsonObject device(json);

            std::string name = device["name"].getValueAsString();
            summary.append(name).append(" - ");

            summary.append("subdevices: ");
            JsonCppWrapper::JsonArray subdevices = device["subdevices"].asArray();
            for (auto subdevice : subdevices) {
                // traverse subdevice properties
                if (subdevice.getType() == json_object) {
                    JsonCppWrapper::JsonObject props = subdevice.asObject();
                    std::string type = props["type"].getValueAsString();
                    summary.append(type).append("  ");
                }
            }
        }
        json_value_free(json);
    }
    return summary;
}


/**
 * Get a list of all zigbee entities connected to the gateway.
 * @param gw phoscon gateway
 * @param qualified name of the zigbee entity (e.g. devices, lights, sensors, ...
 * @return a map of module id and module name pairs
 */
std::map<std::string, JsonCppWrapper::JsonObject> PhosconAPI::getEntityObjects(const PhosconGW& gw, const std::string& qualifier) const {
    std::map<std::string, JsonCppWrapper::JsonObject> entities;

    // send http get api request
    std::string response, content;
    int http_return_code = HttpClient().sendHttpGetRequest(gw.getApiUrl() + qualifier, response, content);

    if (http_return_code == 200) {
        // parse json content
        json_value* json = json_parse(content.c_str(), content.length());

        // traverse json tree; expected is an object with one element for each zigbee entity
        if (json != NULL && json->type == json_object) {
            const json_object_entry* elements = json->u.object.values;
            unsigned int         num_elements = json->u.object.length;
            JsonCppWrapper::JsonNamedValueVector objects = JsonCppWrapper::getNamedValues(elements, num_elements);
            for (const auto& object : objects) {
                entities[object.getName()] = object.getObject();
            }
        }
        json_value_free(json);
    }
    return entities;
}


/**
 * Get the value for the given key path from the phoscon device; the value is converted to a string.
 * The key path is a string containing path segments, separated by ':' characters. The path is defining
 * the traversal through the result of a "Status 0" command to the phoscon device.
 * @param gw phoscon gateway
 * @param device zigbee device id
 * @param path the path to the leaf key value pair. e.g. "subdevices:1:state:power:value" to get the power consumption
 * @return the value of the leaf key value pair
 */
std::string PhosconAPI::getValueFromPath(const PhosconGW& gw, const std::string& device, const std::string& path) const {
    std::string value;

    // send http get api request
    std::string response, content;
    int http_return_code = HttpClient().sendHttpGetRequest(gw.getApiUrl() + "devices/" + device, response, content);

    if (http_return_code == 200) {
        // parse json content
        json_value* json = json_parse(content.c_str(), content.length());

        // split path into segments
        std::vector<std::string> path_segments = getPathSegments(path);

        // declare name comparators for json nodes and json leafs
        struct NameComparator {
            static bool compare_node_names(const std::string& lhs, const std::string& rhs) { return compareNames(lhs, rhs, true); }
            static bool compare_leaf_names(const std::string& lhs, const std::string& rhs) { return compareNames(lhs, rhs, false); }
        };
        
        // traverse path
        const json_object_entry* values = NULL;
        size_t                   length = 0;
        if (path_segments.size() > 1 && json->type == json_object) {
            values = json->u.object.values;
            length = json->u.object.length;
            for (size_t i = 0; i < path_segments.size() - 1; ++i) {
                const JsonCppWrapper::JsonNamedValue node = JsonCppWrapper::getValue(values, length, path_segments[i], NameComparator::compare_node_names);
                if (node.getType() == json_array) {
                    if (i + 1 < path_segments.size() - 1) {
                        std::string path_index = path_segments[++i];
                        int index = 0;
                        if (sscanf(path_index.c_str(), "%d", &index) == 1) {
                            values = node.getArray().getElements()[index]->u.object.values;
                            length = node.getArray().getElements()[index]->u.object.length;
                        }
                    }
                }
                else if (node.getType() == json_object) {
                    values = node.getObject().getElements();
                    length = node.getObject().getNumElements();
                }
                else {
                    break;
                }
            }
        }
        if (values != NULL && length != 0 && path_segments.size() > 0) {
            JsonCppWrapper::JsonNamedValue leaf = JsonCppWrapper::getValue(values, length, path_segments[path_segments.size()-1], NameComparator::compare_leaf_names);
            value = leaf.getValueAsString();
        }
        json_value_free(json);
    }
    return value;
}


/**
 * Get the value for the given name from the given json tree.
 * @param json the json tree
 * @param name the name of the name value pair
 * @return the value of the name value pair
 */
std::string PhosconAPI::getValueFromJson(const json_value* const json, const std::string& name) {
    if (json != NULL) {

        // analyze the json response
        const JsonCppWrapper::JsonNamedValueVector roots = JsonCppWrapper::getNamedValues(json);
        const JsonCppWrapper::JsonNamedValue&      root  = roots[0];
        if (compareNames(root.getName(), name, false)) {
            if (root.getType() == json_object) {
                const json_object_entry* elements = root.getObject().getElements();
                int64_t              num_elements = root.getObject().getNumElements();
                for (int i = 0; i < num_elements; ++i) {
                    const json_object_entry& element = elements[i];
                    const std::string        name    = std::string(element.name);
                    switch (element.value->type) {
                    case json_string:   return JsonCppWrapper::JsonString(&element).getValue();
                    case json_double:   return JsonCppWrapper::JsonDouble(&element).getValueAsString();
                    case json_integer:  return JsonCppWrapper::JsonInt   (&element).getValueAsString();
                    case json_boolean:  return JsonCppWrapper::JsonBool  (&element).getValueAsString();
                    case json_null:     return "null";
                    }
                }
            }
            switch (root.getType()) {
            case json_string:   return root.getString().getValue();
            case json_double:   return root.getDouble().getValueAsString();
            case json_integer:  return root.getInt   ().getValueAsString();
            case json_boolean:  return root.getBool  ().getValueAsString();
            case json_null:     return "null";
            }
        }
    }
    return "";
}


/**
 * Compare phoscon key names.
 * @param name1 the first name to compare
 * @param name2 the second name to compare
 * @param strict false: name extensions with digits are ignored; true: only differences in lower and upper case are ignored
 * @return true, if the two names are considered to be equal; false, if the two names are considered to be different
 */
bool PhosconAPI::compareNames(const std::string& name1, const std::string& name2, const bool strict) {

    // first check if both strings are identical
    if (name1 == name2) {
        return true;
    }

    // extract base names by removing any trailing digits
    std::string base_name1 = name1;
    std::string base_name2 = name2;

    // if the comparison is not strict, consider "Module0" and "Module" as the same name
    if (strict == false) {
        while (base_name1.length() > 0) {
            char last_char = base_name1.at(base_name1.length() - 1);
            if (last_char < '0' || last_char > '9') break;
            base_name1 = base_name1.substr(0, base_name1.length() - 1);
        }
        while (base_name2.length() > 0) {
            char last_char = base_name2.at(base_name2.length() - 1);
            if (last_char < '0' || last_char > '9') break;
            base_name2 = base_name2.substr(0, base_name2.length() - 1);
        }
    }

    // check if lengths of both strings are identical
    if (base_name1.length() == base_name2.length()) {

        // check if both strings are identical, when converted to lower case
        bool same = true;
        for (int i = 0; i < base_name1.length(); ++i) {
            std::string::value_type char1 = base_name1[i];
            std::string::value_type char2 = base_name2[i];
            if (char1 != char2) {
                std::string::value_type char1_lower = std::tolower(char1);
                std::string::value_type char2_lower = std::tolower(char2);
                if (char1_lower != char2_lower) {
                    same = false;
                    break;
                }
            }
        }
        if (same == true) {
            return true;
        }
    }

    return false;
}


/**
 * Segment a given key path into a vector of path segments.
 * The key path is a string containing path segments, separated by ':' characters. The path is defining
 * the traversal through the result of a "Status 0" command to the tasmota device.
 * @param key the key path of the key value pair. e.g. "StatusSNS:ENERGY:Power" to get the power consumption
 * @return a vector containing path segments, e.g. "StatusSNS", "ENERGY", "Power"
 */
std::vector<std::string> PhosconAPI::getPathSegments(const std::string& path) {
    std::vector<std::string> segments;
    std::string::size_type index = 0;
    while (true) {
        std::string::size_type index_next = path.find(':', index);
        if (index_next == std::string::npos) {
            std::string segment = path.substr(index);
            if (segment.size() > 0) {
                segments.push_back(segment);
            }
            break;
        }
        std::string segment = path.substr(index, index_next-index);
        if (segment.size() > 0) {
            segments.push_back(segment);
        }
        index = index_next + 1;
    }
    return segments;
}
