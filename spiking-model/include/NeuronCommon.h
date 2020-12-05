#pragma once

#include <chrono>

namespace embeddedpenguins::neuron::infrastructure
{
    using std::chrono::milliseconds;

    constexpr int PresynapticConnectionsPerNode = 1100;
    constexpr int PostsynapticConnectionsPerNode = 1500;
    constexpr int MaxSynapseStrength = 110;
    constexpr int MinSynapseStrength = -110;
    constexpr int ActivationThreshold = 100;
    constexpr int PostsynapticPlasticityPeriod = 20;
    constexpr int SignalDelayTime = 1;
    constexpr int RefractoryTime = 10;
    constexpr double DecayRate = 0.80;

    const float PostsynapticIncreaseFunction[PostsynapticPlasticityPeriod] = 
        { 1.0, 1.125, 1.250, 1.375, 1.5, 1.625, 1.750, 1.875, 2.0, 2.125, 2.250, 2.375, 2.5, 2.0, 1.750, 1.5, 1.250, 1.0, 1.0, 1.0 };

    const float PostsynapticDecreaseFunction[PostsynapticPlasticityPeriod] = 
        { 1.0, 0.95, 0.90, 0.85, 0.80, 0.75, 0.70, 0.65, 0.60, 0.55, 0.5, 0.58, 0.66, 0.74, 0.82, 0.89, 0.96, 1.0, 1.0, 1.0 };

    enum class Operation
    {
        Spike,          // The NeuronNode indexed by a NeuronOperation.Index received a spike from a synapse.
        Decay,          // The NeuronNode indexed by a NeuronOperation.Index is in the decay phase.
        Refractory      // The NeuronNode indexed by a NeuronOperation.Index is in refractory period after spiking.
    };
}
