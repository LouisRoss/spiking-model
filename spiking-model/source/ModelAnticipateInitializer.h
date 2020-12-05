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
        const Neuron2Dim I1 = Neuron2Dim{0, 10};
        const Neuron2Dim I2 = Neuron2Dim{0, 30};
        const Neuron2Dim Inh1 = Neuron2Dim{0, 9};
        const Neuron2Dim Inh2 = Neuron2Dim{0, 31};
        const Neuron2Dim N1 = Neuron2Dim{10, 10};
        const Neuron2Dim N2 = Neuron2Dim{10, 30};

    public:
        ModelAnticipateInitializer(vector<NeuronNode>& model, json& configuration);
        virtual void Initialize() override;
        virtual void InjectSignal(ProcessCallback<NeuronOperation, NeuronRecord>& callback) override;
    };
}
