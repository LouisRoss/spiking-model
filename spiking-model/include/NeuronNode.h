#pragma once

#include <limits>

#include "NeuronCommon.h"
#include "NeuronConnection.h"
#include "NeuronSynapse.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using std::numeric_limits;

    enum class NeuronType
    {
        Excitatory,
        Inhibitory
    };

    struct NeuronNode
    {
        NeuronConnection PostsynapticConnections[PostsynapticConnectionsPerNode];
        NeuronSynapse Synapses[PresynapticConnectionsPerNode];
        unsigned long int RequiredPostsynapticConnections { };
        unsigned long int RequiredPresynapticConnections { };
        NeuronType Type { NeuronType::Excitatory };
        int Activation { 0 };
        unsigned long long int TickLastSpike { numeric_limits<unsigned long long int>::max() - 1000ULL };
        bool InRefractoryDelay { false };
        bool InDecay { false };
    };
}
