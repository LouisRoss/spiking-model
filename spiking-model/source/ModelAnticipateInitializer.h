#pragma once

#include "ModelNeuronInitializer.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using std::vector;
    using embeddedpenguins::modelengine::ModelEngine;
    using nlohmann::json;
    using embeddedpenguins::neuron::infrastructure::ModelNeuronInitializer;
    using embeddedpenguins::neuron::infrastructure::NeuronNode;
    using embeddedpenguins::neuron::infrastructure::NeuronOperation;
    using embeddedpenguins::neuron::infrastructure::NeuronImplementation;
    using embeddedpenguins::neuron::infrastructure::NeuronRecord;

    //
    // This custom initializer sets up a spiking neuron model for 
    // the 'anticipate' test, which demonstrates STDP over repeated
    // spikes.
    //
    class ModelAnticipateInitializer : public ModelNeuronInitializer
    {
    public:
        ModelAnticipateInitializer(vector<NeuronNode>& model, json& configuration);
        virtual void Initialize() override;
        virtual void InjectSignal(ProcessCallback<NeuronOperation, NeuronRecord>& callback) override;
    };
}
