#pragma once

#include "sdk/ModelUi.h"

#include "sdk/ModelRunner.h"
#include "NeuronModelHelper.h"
#include "CpuModelCarrier.h"
#include "NeuronOperation.h"
#include "NeuronImplementation.h"
#include "NeuronRecord.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using embeddedpenguins::modelengine::sdk::ModelRunner;
    using embeddedpenguins::core::neuron::model::ModelUi;

    class CpuModelUi : public ModelUi<ModelRunner<NeuronOperation, NeuronImplementation, NeuronModelHelper<CpuModelCarrier>, NeuronRecord>, NeuronModelHelper<CpuModelCarrier>>
    {
        unsigned long int modelSize_ {};
        string legend_ {};

    public:
        CpuModelUi(ModelRunner<NeuronOperation, NeuronImplementation, NeuronModelHelper<CpuModelCarrier>, NeuronRecord>& modelRunner, NeuronModelHelper<CpuModelCarrier>& helper) :
            ModelUi(modelRunner, helper)
        {
            modelSize_ = helper.Model().ModelSize();
        }

        virtual char EmitToken(unsigned long neuronIndex) override
        {
            if (neuronIndex >= modelSize_) return '=';
            
            auto activation = helper_.GetNeuronActivation(neuronIndex);
            return MapIntensity(activation);
        }

        virtual const string& Legend() override
        {
            return legend_;
        }

    private:
        char MapIntensity(int activation)
        {
            static int cutoffs[] = {2,5,15,50};

            if (activation < cutoffs[0]) return ' ';
            if (activation < cutoffs[1]) return '.';
            if (activation < cutoffs[2]) return '*';
            if (activation < cutoffs[3]) return 'o';
            return 'O';
        }
    };
}
