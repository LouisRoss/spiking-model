#pragma once

#include <limits>
namespace embeddedpenguins::neuron::infrastructure
{
    using std::numeric_limits;

    struct NeuronConnection
    {
        unsigned long long int PostsynapticNeuron { numeric_limits<unsigned long long int>::max() };
        int Synapse { 0 };
        bool IsUsed { false };
    };
}
