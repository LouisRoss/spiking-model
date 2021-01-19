#pragma once

#include <iostream>
#include <fstream>

#include "sys/stat.h"
#include "nlohmann/json.hpp"

#include "sdk/IModelPersister.h"

#include "NeuronNode.h"

#include "SonataModelRepository.h"
#include "SonataNodeModelLoader.h"
#include "SonataEdgeModelLoader.h"
#include "SonataModelLoader.h"

namespace embeddedpenguins::neuron::infrastructure::persistence::sonata
{
    using std::cout;
    using std::ifstream;
    using nlohmann::json;
    using embeddedpenguins::neuron::infrastructure::NeuronNode;

    //
    //
    class SonataModelPersister : public IModelPersister<NeuronNode>
    {
        string sonataConfigurationFilePath_;
        SonataModelRepository& sonataRepository_;

    public:
        const SonataModelRepository& SonataRepository() const { return sonataRepository_; }
        
    public:
        SonataModelPersister(const string& path, SonataModelRepository& sonataRepository) :
            sonataConfigurationFilePath_(path),
            sonataRepository_(sonataRepository)
        {
        }

        bool LoadConfiguration() override
        {
            json simulationConfiguration {};
            if (!LoadConfigurationFile(sonataConfigurationFilePath_, simulationConfiguration))
            {
                cout << "Unable to load simulation configuration, cannot proceed\n";
                return false;
            }
            sonataRepository_.AddConfiguration(SIMULATION_CONFIGURATION_NAME, simulationConfiguration);

            auto networkFile = sonataRepository_.GetStringFromNamedConfiguration(SIMULATION_CONFIGURATION_NAME, NETWORK_CONFIGURATION_NAME);
            json circuitConfiguration {};
            if (!LoadConfigurationFile(networkFile, circuitConfiguration))
            {
                cout << "Unable to load network configuration, cannot proceed\n";
                return false;
            }
            sonataRepository_.AddConfiguration(NETWORK_CONFIGURATION_NAME, circuitConfiguration);

            auto nodesetsFile = sonataRepository_.GetStringFromNamedConfiguration(SIMULATION_CONFIGURATION_NAME, NODE_SETS_CONFIGURATION_NAME);
            json nodesetsConfiguration {};
            if (LoadConfigurationFile(nodesetsFile, nodesetsConfiguration))
                sonataRepository_.AddConfiguration(NODE_SETS_CONFIGURATION_NAME, nodesetsConfiguration);

            return true;
        }

        bool ReadModel(vector<NeuronNode>& model, json& configuration) override
        {
            auto& circuitConfiguration = sonataRepository_.GetConfiguration(NETWORK_CONFIGURATION_NAME);
            auto networks = circuitConfiguration[NETWORK_NETWORKS];
            auto nodes = networks[NETWORK_NETWORKS_NODES];
            auto edges = networks[NETWORK_NETWORKS_EDGES];
            if (!nodes.is_array() || !edges.is_array())
            {
                cout 
                    << "Circuit configuration does not contain correct json elements for " 
                    << NETWORK_NETWORKS << "\\" << NETWORK_NETWORKS_NODES 
                    << " and " 
                    << NETWORK_NETWORKS << "\\" << NETWORK_NETWORKS_EDGES << '\n';
                return false;
            }

            cout << "Loaded nodes and edges from circuit configuration\n";
            SonataNodeModelLoader nodeLoader(sonataRepository_);
            nodeLoader.BuildPopulations(nodes);
            SonataEdgeModelLoader edgeLoader(sonataRepository_);
            edgeLoader.MapEdges(edges);

            SonataModelLoader modelLoader(nodeLoader.Populations(), edgeLoader.ModelConnections());
            modelLoader.LoadModel(model, configuration);

            return true;
        }

    private:
        bool LoadConfigurationFile(const string& path, json& configuration)
        {
            auto settingsFile = path;

            if (settingsFile.length() < 5 || settingsFile.substr(settingsFile.length()-5, settingsFile.length()) != ".json")
                settingsFile += ".json";

            struct stat buffer;   
            if (!(stat (settingsFile.c_str(), &buffer) == 0))
            {
                cout << "Configuration file " << settingsFile << " does not exist\n";
                return false;
            }

            cout << "LoadConfiguration from " << settingsFile << "\n";
            ifstream settings(settingsFile);
            settings >> configuration;

            return true;
        }
    };
}
