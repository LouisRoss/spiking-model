#pragma once

#include "ModelNeuronInitializer.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using nlohmann::json;
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
        ModelLayerInitializer(json& configuration, MODELHELPERTYPE helper) :
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

            this->strength_ = this->configuration_["Model"]["InitialSynapticStrength"];
            for (auto row = 0; row < this->height_ - 1; row++)
            {
                InitializeARow(row, row + 1);
            }
            InitializeARow(this->height_ - 1, 0);
        }

        virtual void InjectSignal(ProcessCallback<NeuronOperation, NeuronRecord>& callback) override
        {
            /*
            for (int i = 1; i < 6; i++)
            {
                callback(NeuronOperation(1, Operation::Spike, 0), (i*3)+0);
                callback(NeuronOperation(2, Operation::Spike, 0), (i*3)-1);
                callback(NeuronOperation(3, Operation::Spike, 0), (i*3)-2);
                callback(NeuronOperation(4, Operation::Spike, 0), (i*3)-3);
                callback(NeuronOperation(5, Operation::Spike, 0), (i*3)-4);
            }
            */
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
