#pragma once

#include <string>
#include <map>
#include <iostream>
#include "nlohmann/json.hpp"

namespace embeddedpenguins::neuron::infrastructure::persistence::sonata
{
    using std::string;
    using std::map;
    using std::cout;
    using nlohmann::json;

    constexpr const char* SIMULATION_CONFIGURATION_NAME = "simulation";
    constexpr const char* NETWORK_CONFIGURATION_NAME = "network";
    constexpr const char* NODE_SETS_CONFIGURATION_NAME = "node_sets_file";
    constexpr const char* NETWORK_NETWORKS = "networks";
    constexpr const char* NETWORK_NETWORKS_NODES = "nodes";
    constexpr const char* NETWORK_INPUTS = "inputs";
    constexpr const char* GROUP_NODES = "/nodes";
    constexpr const char* NETWORK_NETWORKS_NODES_FILE = "nodes_file";
    constexpr const char* NETWORK_NETWORKS_NODE_TYPES_FILE = "node_types_file";
    constexpr const char* DATASET_NODE_TYPE_ID = "node_type_id";
    constexpr const char* NETWORK_NETWORKS_EDGES = "edges";
    constexpr const char* GROUP_EDGES = "/edges";
    constexpr const char* NETWORK_NETWORKS_EDGES_FILE = "edges_file";
    constexpr const char* NETWORK_NETWORKS_EDGES_TYPES_FILE = "edge_types_file";
    constexpr const char* DATASET_EDGE_NSYNS = "nsyns";
    constexpr const char* DATASET_EDGE_GROUP_ID = "edge_group_id";
    constexpr const char* DATASET_EDGE_GROUP_INDEX = "edge_group_index";
    constexpr const char* DATASET_EDGE_TYPE_ID = "edge_type_id";
    constexpr const char* DATASET_SOURCE_NODE_ID = "source_node_id";
    constexpr const char* DATASET_TARGET_NODE_ID = "target_node_id";
    constexpr const char* ATTRIBUTE_NODE_POPULATION = "node_population";
    constexpr const char* GROUP_PROCESSING = "/processing";
    constexpr const char* GROUP_SPIKE_TRAIN = "spike_train";
    constexpr const char* DATASET_DATA = "data";

    //
    // Encapsulate the json parts of the sonata configuration.  Provide
    // access to configuration elements, including sub-json objects.
    // Expand macros where found.
    //
    class SonataModelRepository
    {
        const string configurationDirectory_ {};
        map<string, string> macroExpansions_ {};
        map<string, json> controlConfigurations_ {};

    public:
        SonataModelRepository(const string& configurationDirectory) :
            configurationDirectory_(ExtractConfigurationDirectory(configurationDirectory))
        {

        }

        void AddConfiguration(const string& configurationName, json& configuration)
        {
            ScanForMacros(configuration);
            controlConfigurations_[configurationName] = configuration;
        }

        void AddMacroDefinition(const string& key, const string& macroExpansion)
        {
            macroExpansions_[key] = macroExpansion;
        }

        const json& GetConfiguration(const string& configurationName) const
        {
            static json nullConfiguration {};

            auto it = controlConfigurations_.find(configurationName);
            if (it == controlConfigurations_.end())
                return nullConfiguration;

            return it->second;
        }

        const string GetStringFromNamedConfiguration(const string& configurationName, const string& key) const
        {
            const auto& configuration = GetConfiguration(configurationName);
            return GetStringFromConfiguration(configuration, key);
        }

        const string GetStringFromConfiguration(const json& configuration, const string& key) const
        {
            static string nullString {};

            auto result = configuration[key];
            if (!result.is_string())
                return nullString;
            else
                return Expand(result.get<string>());
        }

    private:
        string ExtractConfigurationDirectory(const string& configurationPath)
        {
            string configurationDirectory(".");
            auto lastslash = configurationPath.find_last_of("/\\");
            if (lastslash != string::npos)
                configurationDirectory = configurationPath.substr(0, lastslash);

            return configurationDirectory;
        }

        string Expand(const string& value) const
        {
            string expanded(value);
            auto replaced = false;
            for (auto& macro : macroExpansions_)
            {
                auto location = expanded.find(macro.first);
                if (location != string::npos)
                {
                    expanded.replace(location, macro.first.length(), macro.second);
                    replaced = true;
                }
            }

            if (replaced) return Expand(expanded);
            return expanded;
        }

        void ScanForMacros(json& configuration)
        {
            auto manifest = configuration["manifest"];
            for (auto it = manifest.begin(); it != manifest.end(); ++it)
            {
                auto key = it.key();
                if (key[0] != '$')
                    continue;

                if (it->is_string())
                {
                    auto macroExpansion = manifest[key].get<string>();
                    if (macroExpansion == "${configdir}")
                        macroExpansion = configurationDirectory_;
                    cout << "Capturing macro " << key << " = " << macroExpansion << '\n';
                    macroExpansions_[key] = macroExpansion;
                }
            }
        }
    };
}
