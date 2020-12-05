#pragma once

#include <limits>

namespace embeddedpenguins::neuron::infrastructure
{
    using std::numeric_limits;

    enum class SynapseType
    {
        Excitatory,
        Inhibitory,
        Attention
    };

    struct NeuronSynapse
    {
        int Strength { 0 };
        unsigned int TickLastSignal { numeric_limits<unsigned int>::max() - 1000U };
        SynapseType Type { SynapseType::Excitatory };
        bool IsUsed { false };

        static std::string ToString(SynapseType type)
        {
            switch (type)
            {
                case SynapseType::Excitatory:
                    return "Excitatory";
                case SynapseType::Inhibitory:
                    return "Inhibitory";
                case SynapseType::Attention:
                    return "Attention";
                default:
                    break;
            }

            return "<unknown>";
        }
    };
}
