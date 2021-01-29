#pragma once

#include <iostream>
#include <vector>
#include <tuple>

#include "nlohmann/json.hpp"

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

    template<class ModelCarrier>
    class NeuronModelHelper
    {
        ModelCarrier& carrier_;
        json& configuration_;

    public:
        NeuronModelHelper(ModelCarrier& carrier, json& configuration) :
            carrier_(carrier),
            configuration_(configuration)
        {
            
        }

        vector<NeuronNode>& Model() { return carrier_.Model; }
        ModelCarrier& Carrier() { return carrier_; }
        const json& Configuration() const { return configuration_; }

        void InitializeModel(unsigned long int modelSize = 0)
        {
            auto size = modelSize;
            if (size == 0)
                size = (configuration_["Model"]["ModelSize"]).get<int>();

            carrier_.Model.resize(size);

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

        void WireInput(unsigned long int sourceNodeIndex, int synapticWeight)
        {
            auto& sourceNode = carrier_.Model[sourceNodeIndex];

            sourceNode.Synapses[0].IsUsed = true;
            sourceNode.Synapses[0].Strength = synapticWeight;
            sourceNode.Synapses[0].Type = sourceNode.Type == NeuronType::Excitatory ? SynapseType::Excitatory : SynapseType::Inhibitory;

            sourceNode.RequiredPostsynapticConnections++;
        }

        void Wire(unsigned long long int sourceNodeIndex, unsigned long long int targetNodeIndex, int synapticWeight)
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
