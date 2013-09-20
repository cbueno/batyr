#include "configuration.h"

#include <fstream>
#include <algorithm>

#include "../lib/ini-parser/ini.hpp"

using namespace Batyr;


Configuration::Configuration(const std::string & configFile)
    :   http_port(9090),        // default value
        num_worker_threads(2),  // default value
        max_age_done_jobs(600)  // default value
{
    parse(configFile);
}

Layer::Ptr
Configuration::getLayer(const std::string & _layer) const
{
    Layer::Ptr layer;
    try {
        layer = layers.at(_layer);
    }
    catch (std::out_of_range) {
        throw ConfigurationError("Layer " + _layer + " not found");
    }
    return layer;
}


std::vector<Layer::Ptr>
Configuration::getOrderedLayers() const
{    
    std::vector< Layer::Ptr > orderedLayers;
    orderedLayers.reserve(layers.size());

    for (auto const kv : layers) {
        orderedLayers.push_back(kv.second);
    }

    std::sort(orderedLayers.begin(), orderedLayers.end(), [](Layer::Ptr l1, Layer::Ptr l2) {
        return l2->name < l1->name;
    });

    return orderedLayers;
}


void 
throwUnknownSetting(const std::string & sectionName, const std::string & settingName)
{
    throw ConfigurationError("Unknown option \""+settingName+
                    "\" in section \""+sectionName+"\"");
}

void 
throwInvalidValue(const std::string & sectionName, const std::string & settingName,
        const std::string & value)
{
    throw ConfigurationError("Invalid value for \""+settingName+
                    "\" in section \""+sectionName+"\": \""+value+"\"");
}


std::string
trim(const std::string &s, const std::string & characters = "\"' \t\r\n")
{
  size_t sec_start = s.find_first_not_of(characters);
  size_t sec_end = s.find_last_not_of(characters);
  return s.substr(sec_start, sec_end - sec_start + 1);
}


int
valueToInt(const std::string & s, bool & ok)
{
    int i;
    try {
        i = std::stoi(trim(s));
    }
    catch (std::exception) {
        ok = false;
    }
    return i;
}


void
Configuration::parse(const std::string & configFile)
{
    std::ifstream ifs;
    ifs.open(configFile);
    if (!ifs.is_open()) {
        throw ConfigurationError("Could not open configfile at "+configFile);
    }

    try {
        
        INI::Parser parser(ifs);
        INI::Level toplevel = parser.top();

        bool ok = true;
        // parse all section seperate
        for(auto const sectionPair : toplevel.sections) {
            if (sectionPair.first == "HTTP") {
                for(auto const valuePair : sectionPair.second.values) {
                    if (valuePair.first == "port") {
                        http_port = valueToInt(valuePair.second, ok);
                        if (!ok) {
                            throwInvalidValue(sectionPair.first,
                                        valuePair.first,
                                        valuePair.second);
                        }
                    }
                    else {
                        throwUnknownSetting(sectionPair.first, valuePair.first);
                    }
                }

            }
            else if (sectionPair.first == "MAIN") {
                for(auto const valuePair : sectionPair.second.values) {
                    if (valuePair.first == "num_worker_threads") {
                        int _num_worker_threads = valueToInt(valuePair.second, ok);
                        if (!ok) {
                            throwInvalidValue(sectionPair.first,
                                        valuePair.first,
                                        valuePair.second);
                        }
                        if (_num_worker_threads < 1) {
                            throw ConfigurationError("At least one worker thread is required.");
                        }
                        num_worker_threads = _num_worker_threads;
                    }
                    else if (valuePair.first == "max_age_done_jobs") {
                        int _max_age_done_jobs = valueToInt(valuePair.second, ok);
                        if (!ok) {
                            throwInvalidValue(sectionPair.first,
                                        valuePair.first,
                                        valuePair.second);
                        }
                        if (_max_age_done_jobs < 1) {
                            throw ConfigurationError("max_age_done_jobs must be a positive value.");
                        }
                        max_age_done_jobs = _max_age_done_jobs;
                    }
                    else {
                        throwUnknownSetting(sectionPair.first, valuePair.first);
                    }
                }

            }
            else if (sectionPair.first == "LAYERS") {
                for(auto const layerSectionPair : sectionPair.second.sections) {
                    auto layer = std::make_shared<Layer>();
                    layer->name = layerSectionPair.first;

                    // TODO: collect layer infos
                    for(auto const layerValuePair: layerSectionPair.second.values) {
                        if (layerValuePair.first == "description") {
                            layer->description = layerValuePair.second;
                        }
                        else {
                            throwUnknownSetting(layerSectionPair.first, layerValuePair.first);
                        }
                    }

                    // check for missing mantatory settings
                    // TODO

                    layers[layer->name] = layer;
                }

            }
            else if (sectionPair.first == "LOGGING") {
            }
            else {
                throw ConfigurationError("Unknown section: \""+sectionPair.first+"\"");
            }
        }

        // check for missing mantatory settings
        // TODO

    }
    catch( std::runtime_error &e) {
        throw ConfigurationError(e.what());
    }
    catch( std::exception ) {
        throw ConfigurationError("Could not parse configuraton file");
    }
}

