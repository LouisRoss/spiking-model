#pragma once

#include <iostream>
#include <vector>
#include <tuple>

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

    class NeuronModelHelper
    {
        vector<NeuronNode>& model_;

    public:
        NeuronModelHelper(vector<NeuronNode>& model) :
            model_(model)
        {
            
        }

        void WireInput(unsigned long int sourceNodeIndex, int synapticWeight)
        {
            auto& sourceNode = model_[sourceNodeIndex];

            sourceNode.Synapses[0].IsUsed = true;
            sourceNode.Synapses[0].Strength = synapticWeight;
            sourceNode.Synapses[0].Type = sourceNode.Type == NeuronType::Excitatory ? SynapseType::Excitatory : SynapseType::Inhibitory;

            sourceNode.RequiredPostsynapticConnections++;
        }

        void Wire(unsigned long long int sourceNodeIndex, unsigned long long int targetNodeIndex, int synapticWeight)
        {
            auto& sourceNode = model_[sourceNodeIndex];
            auto sourceSynapseIndex = FindNextUnusedSourceConnection(sourceNode);
            auto& targetNode = model_[targetNodeIndex];
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

        tuple<unsigned long int, unsigned long int> FindRequiredSynapseCounts()
        {
            unsigned long int postsynapticConnections {};
            unsigned long int presynapticConnections {};

            for (const auto& node : model_)
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
