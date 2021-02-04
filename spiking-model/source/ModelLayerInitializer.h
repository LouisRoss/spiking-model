#pragma once

#include "ModelEngineCommon.h"

#include "ModelNeuronInitializer.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using embeddedpenguins::modelengine::ConfigurationUtilities;
    using embeddedpenguins::neuron::infrastructure::ModelNeuronInitializer;
    using embeddedpenguins::neuron::infrastructure::NeuronOperation;
    using embeddedpenguins::neuron::infrastructure::NeuronRecord;
    using embeddedpenguins::modelengine::threads::ProcessCallback;

    //
    // This custom initializer sets up a spiking neuron model for 
    // the 'layer' test, which demonstrates a continuous wave action
    // in layers.
    //
    template<class MODELHELPERTYPE>
    class ModelLayerInitializer : public ModelNeuronInitializer<MODELHELPERTYPE>
    {
    public:
        ModelLayerInitializer(ConfigurationUtilities& configuration, MODELHELPERTYPE helper) :
            ModelNeuronInitializer<MODELHELPERTYPE>(configuration, helper)
        {
        }

        virtual void Initialize() override
        {
            this->helper_.InitializeModel();

            this->strength_ = 101;
            this->InitializeAnInput(0, 1);
            this->InitializeAnInput(0, 2);
            this->InitializeAnInput(0, 3);
            this->InitializeAnInput(0, 4);
            this->InitializeAnInput(0, 5);

            this->strength_ = this->configuration_.Configuration()["Model"]["InitialSynapticStrength"];
            for (auto row = 0; row < this->height_ - 1; row++)
            {
                InitializeARow(row, row + 1);
            }
            InitializeARow(this->height_ - 1, 0);
        }

    private:
        void InitializeARow(int row, int destRow)
        {
            for (auto column = 0; column < this->width_; column++)
                for (auto destCol = 0; destCol < this->width_; destCol++)
                    this->InitializeAConnection(row, column, destRow, destCol);
        }
    };
}
