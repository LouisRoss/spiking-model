#pragma once

#include "ModelNeuronInitializer.h"
#include "ModelEngineCommon.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using nlohmann::json;
    using embeddedpenguins::neuron::infrastructure::ModelNeuronInitializer;
    using embeddedpenguins::neuron::infrastructure::NeuronOperation;
    using embeddedpenguins::neuron::infrastructure::NeuronRecord;
    using embeddedpenguins::modelengine::threads::ProcessCallback;
    using embeddedpenguins::modelengine::ConfigurationUtilities;

    //
    // This custom initializer sets up a spiking neuron model for 
    // the 'anticipate' test, which demonstrates STDP over repeated
    // spikes.
    //
    template<class MODELHELPERTYPE>
    class ModelAnticipateInitializer : public ModelNeuronInitializer<MODELHELPERTYPE>
    {
/*
        const Neuron2Dim I1 = Neuron2Dim{0, 10};
        const Neuron2Dim I2 = Neuron2Dim{0, 30};
        const Neuron2Dim Inh1 = Neuron2Dim{0, 9};
        const Neuron2Dim Inh2 = Neuron2Dim{0, 31};
        const Neuron2Dim N1 = Neuron2Dim{10, 10};
        const Neuron2Dim N2 = Neuron2Dim{10, 30};
*/
    public:
        ModelAnticipateInitializer(ConfigurationUtilities& configuration, MODELHELPERTYPE helper) :
            ModelNeuronInitializer<MODELHELPERTYPE>(configuration, helper)
        {
        }

        virtual void Initialize() override
        {
            const Neuron2Dim I1 { this->ResolveNeuron("I1") };
            const Neuron2Dim I2 { this->ResolveNeuron("I2") };
            const Neuron2Dim Inh1  { this->ResolveNeuron("Inh1") };
            const Neuron2Dim Inh2  { this->ResolveNeuron("Inh2") };
            const Neuron2Dim N1  { this->ResolveNeuron("N1") };
            const Neuron2Dim N2  { this->ResolveNeuron("N2") };

            this->helper_.InitializeModel();

            this->strength_ = 101;
            this->InitializeAConnection(I1, N1);
            this->strength_ = 51;
            this->InitializeAConnection(N2, N1);

            this->strength_ = 101;
            this->InitializeAConnection(I2, N2);
            this->strength_ = 51;
            this->InitializeAConnection(N1, N2);

            this->strength_ = 101;
            this->InitializeAnInput(I1);
            this->InitializeAnInput(I2);

            this->strength_ = 101;
            this->SetInhibitoryNeuronType(Inh1);
            this->InitializeAConnection(N1, Inh1);
            this->InitializeAConnection(Inh1, I1);

            this->strength_ = 101;
            this->SetInhibitoryNeuronType(Inh2);
            this->InitializeAConnection(N2, Inh2);
            this->InitializeAConnection(Inh2, I2);
        }
    };
}
