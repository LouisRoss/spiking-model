#pragma once

#include <iostream>
#include <vector>
#include <tuple>

#include "nlohmann/json.hpp"

#include "ConfigurationRepository.h"

#include "NeuronCommon.h"
#include "NeuronNode.h"
#include "NeuronConnection.h"
#include "NeuronSynapse.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using std::cout;
    using std::vector;
    using std::tuple;
    using std::make_tuple;

    using nlohmann::json;

    using ::embeddedpenguins::core::neuron::model::ConfigurationRepository;

    template<class ModelCarrier>
    class NeuronModelHelper
    {
        ModelCarrier& carrier_;
        const ConfigurationRepository& configuration_;

        unsigned long int width_ { 100 };
        unsigned long int height_ { 100 };
        unsigned long long int maxIndex_ { };

    public:
        NeuronModelHelper(ModelCarrier& carrier, const ConfigurationRepository& configuration) :
            carrier_(carrier),
            configuration_(configuration)
        {
            
        }

        ModelCarrier& Model() { return carrier_; }
        //ModelCarrier& Carrier() { return carrier_; }
        const json& Configuration() const { return configuration_.Configuration(); }
        const ConfigurationRepository& Repository() const { return configuration_; }
        const unsigned long int Width() const { return width_; }
        const unsigned long int Height() const { return height_; }

        bool AllocateModel(unsigned long int modelSize = 0)
        {
            if (!CreateModel(modelSize))
            {
                cout << "Unable to create model of size " << modelSize << "\n";
                return false;
            }

            return true;
        }

        void InitializeModel(unsigned long int modelSize = 0)
        {
            LoadOptionalDimensions();

            auto longTimeInThePast = numeric_limits<unsigned long long int>::max() - 1000ULL;
            for (auto& neuron : carrier_.Model)
            {
                neuron.TickLastSpike = longTimeInThePast;

                for (auto& synapse : neuron.Synapses)
                {
                    synapse.Strength = 0;
                    synapse.IsUsed = false;
                    synapse.TickLastSignal = longTimeInThePast;
                }
                for (auto& connection : neuron.PostsynapticConnections)
                {
                    connection.PostsynapticNeuron = -1LL;
                    connection.Synapse = -1;
                }
            }
        }

        unsigned long long int GetIndex(const int row, const int column) const
        {
            return row * width_ + column;
        }

        void WireInput(unsigned long int sourceNodeIndex, int synapticWeight)
        {
            auto& sourceNode = carrier_.Model[sourceNodeIndex];

            sourceNode.Synapses[0].IsUsed = true;
            sourceNode.Synapses[0].Strength = synapticWeight;
            sourceNode.Synapses[0].Type = sourceNode.Type == NeuronType::Excitatory ? SynapseType::Excitatory : SynapseType::Inhibitory;

            sourceNode.RequiredPostsynapticConnections++;
        }

        void Wire(unsigned long int sourceNodeIndex, unsigned long int targetNodeIndex, int synapticWeight)
        {
            auto& sourceNode = carrier_.Model[sourceNodeIndex];
            auto sourceSynapseIndex = FindNextUnusedSourceConnection(sourceNode);
            auto& targetNode = carrier_.Model[targetNodeIndex];
            auto targetSynapseIndex = FindNextUnusedTargetSynapse(targetNode);

            if (sourceSynapseIndex != -1 && targetSynapseIndex != -1)
            {
                sourceNode.PostsynapticConnections[sourceSynapseIndex].IsUsed = true;
                sourceNode.PostsynapticConnections[sourceSynapseIndex].PostsynapticNeuron = targetNodeIndex;
                sourceNode.PostsynapticConnections[sourceSynapseIndex].Synapse = targetSynapseIndex;
                targetNode.Synapses[targetSynapseIndex].IsUsed = true;
                targetNode.Synapses[targetSynapseIndex].Strength = synapticWeight;
                targetNode.Synapses[targetSynapseIndex].Type = sourceNode.Type == NeuronType::Excitatory ? SynapseType::Excitatory : SynapseType::Inhibitory;
            }

            sourceNode.RequiredPostsynapticConnections++;
            targetNode.RequiredPresynapticConnections++;
        }

        NeuronType GetNeuronType(const unsigned long long int source) const
        {
            return carrier_.Model[source].Type;
        }

        void SetExcitatoryNeuronType(const unsigned long long int source)
        {
            carrier_.Model[source].Type = NeuronType::Excitatory;
        }

        void SetInhibitoryNeuronType(const unsigned long long int source)
        {
            carrier_.Model[source].Type = NeuronType::Inhibitory;
        }
        
        tuple<unsigned long int, unsigned long int> FindRequiredSynapseCounts()
        {
            unsigned long int postsynapticConnections {};
            unsigned long int presynapticConnections {};

            for (const auto& node : carrier_.Model)
            {
                postsynapticConnections = std::max(postsynapticConnections, node.RequiredPostsynapticConnections);
                presynapticConnections = std::max(presynapticConnections, node.RequiredPresynapticConnections);
            }

            if (postsynapticConnections > PostsynapticConnectionsPerNode)
                cout << "Insufficient postsynaptic space allocated: need " << postsynapticConnections << " have " << PostsynapticConnectionsPerNode << '\n';
            if (presynapticConnections > PresynapticConnectionsPerNode)
                cout << "Insufficient presynaptic space allocated: need " << presynapticConnections << " have " << PresynapticConnectionsPerNode << '\n';

            return make_tuple(postsynapticConnections, presynapticConnections);
        }

    private:
        //
        // Allocate memory for the model.
        // NOTE: Only to be called from the main process, not a load library.
        //
        bool CreateModel(unsigned long int modelSize)
        {
            auto size = modelSize;
            if (size == 0)
            {
                const json& modelJson = configuration_.Configuration()["Model"];
                if (!modelJson.is_null() && modelJson.contains("ModelSize"))
                {
                    const json& modelSizeJson = modelJson["ModelSize"];
                    if (modelSizeJson.is_number_unsigned())
                        size = modelSizeJson.get<unsigned int>();
                }
            }

            if (size == 0)
            {
                cout << "No model size configured or supplied, initializer cannot create model\n";
                carrier_.Valid = false;
                return false;
            }

            carrier_.Model.resize(size);

            return true;
        }

        void LoadOptionalDimensions()
        {
            // Override the dimension defaults if configured.
            const json& configuration = configuration_.Configuration();
            auto& modelSection = configuration["Model"];
            if (!modelSection.is_null() && modelSection.contains("Dimensions"))
            {
                auto dimensionElement = modelSection["Dimensions"];
                if (dimensionElement.is_array())
                {
                    auto dimensionArray = dimensionElement.get<std::vector<int>>();
                    width_ = dimensionArray[0];
                    height_ = dimensionArray[1];
                }
            }

            maxIndex_ = width_ * height_;
        }

        int FindNextUnusedSourceConnection(NeuronNode& sourceNeuron) const
        {
            for (auto i = 0; i < PostsynapticConnectionsPerNode; i++)
            {
                if (!sourceNeuron.PostsynapticConnections[i].IsUsed) return i;
            }

            return -1;
        }

        int FindNextUnusedTargetSynapse(NeuronNode& targetNeuron) const
        {
            for (auto i = 0; i < PresynapticConnectionsPerNode; i++)
            {
                if (!targetNeuron.Synapses[i].IsUsed) return i;
            }

            return -1;
        }
    };
}
