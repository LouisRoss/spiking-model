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
    constexpr int PostsynapticPlasticityPeriod = 30;
    constexpr int SignalDelayTime = 7;
    constexpr int RefractoryTime = 7;
    constexpr double DecayRate = 0.60;
    constexpr int DecayProcessRate = 1;

    const float PostsynapticIncreaseFunction[PostsynapticPlasticityPeriod] = 
        { 2.0, 2.0, 2.0, 2.0, 1.800, 1.800, 1.650, 1.400, 1.400, 1.430, 1.385, 1.385, 1.360, 1.310, 1.310, 1.265, 1.200, 1.200, 1.140, 1.115, 1.115, 1.100, 1.085, 1.085, 1.060, 1.030, 1.030, 1.020, 1.010, 1.010 };

    const float PostsynapticDecreaseFunction[PostsynapticPlasticityPeriod] = 
        { 1.0, 0.95, 0.95, 0.90, 0.85, 0.85, 0.80, 0.75, 0.75, 0.70, 0.65, 0.65, 0.60, 0.55, 0.55, 0.5, 0.58, 0.58, 0.66, 0.74, 0.74, 0.82, 0.89, 0.89, 0.96, 1.0, 1.0, 1.0, 1.0, 1.0 };

    enum class Operation
    {
        Spike,          // The NeuronNode indexed by a NeuronOperation.Index received a spike from a synapse.
        Decay,          // The NeuronNode indexed by a NeuronOperation.Index is in the decay phase.
        Refractory      // The NeuronNode indexed by a NeuronOperation.Index is in refractory period after spiking.
    };
}

namespace embeddedpenguins::core::neuron::model
{
    constexpr int SynapticConnectionsPerNode = embeddedpenguins::neuron::infrastructure::PostsynapticConnectionsPerNode;
}

