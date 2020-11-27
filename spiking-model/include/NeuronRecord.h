#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <limits>
#include <tuple>
#include "NeuronCommon.h"
#include "NeuronNode.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using std::string;
    using std::ostringstream;
    using std::vector;
    using std::numeric_limits;
    using std::tuple;

    enum class NeuronRecordType
    {
        InputSignal,
        Decay,
        Spike,
        Refractory
    };

    struct NeuronRecordConnection
    {
        unsigned long long int NeuronIndex { };
        unsigned int SynapseIndex { };
    };

    struct NeuronRecordSynapse
    {
        unsigned int SynapseIndex { numeric_limits<unsigned int>::max() };
        int Strength { 0 };
    };

    struct NeuronRecord
    {
        NeuronRecordType Type { NeuronRecordType::Refractory };
        unsigned long long int NeuronIndex { };
        int Activation { 0 };
        vector<NeuronRecordConnection> Connections { };
        NeuronRecordSynapse Synapse { };

        NeuronRecord(NeuronRecordType type, unsigned long long int neuronIndex, unsigned int synapseIndex, const NeuronNode& neuronNode) :
            Type(type),
            NeuronIndex(neuronIndex),
            Activation(neuronNode.Activation)
        {
            switch (type)
            {
                case NeuronRecordType::InputSignal:
                    Synapse.SynapseIndex = synapseIndex;
                    Synapse.Strength = neuronNode.Synapses[synapseIndex].Strength;
                    break;

                case NeuronRecordType::Decay:
                    break;

                case NeuronRecordType::Spike:
#ifdef ALLCONNECTIONS
                    for (auto& connection : neuronNode.PostsynapticConnections)
                    {
                        if (connection.IsUsed) 
                            Connections.push_back(NeuronRecordConnection { connection.PostsynapticNeuron, connection.Synapse });
                    }
#endif
                    break;

                case NeuronRecordType::Refractory:
                    break;
                    
                default:
                    break;
            }
        }

        static const string Header()
        {
            ostringstream header;
            header << "Neuron-Event-Type,Neuron-Index,Neuron-Activation,Synapse-Index,Synapse-Strength";
#ifdef ALLCONNECTIONS
            for (auto i = 0; i < PresynapticConnectionsPerNode; i++)
                header << ",Synapse" << i << "-Signaled" << ",Synapse-" << i << "-Strength";
#endif
            return header.str();
        }

        const string Format()
        {
            ostringstream row;
            row << (int)Type << "," << NeuronIndex << "," << Activation << ",";
            switch (Type)
            {
                case NeuronRecordType::InputSignal:
                    row << Synapse.SynapseIndex << ',' << Synapse.Strength;
                    break;

                case NeuronRecordType::Decay:
                    row << "N/A,N/A";
                    break;

                case NeuronRecordType::Spike:
                    row << "N/A,N/A";
#ifdef ALLCONNECTIONS
                    // TODO
#endif
                    break;

                case NeuronRecordType::Refractory:
                    row << "N/A,N/A";
                    break;
                    
                default:
                    row << "N/A,N/A";
                    break;
            }

            return row.str();
        }
    };
}
