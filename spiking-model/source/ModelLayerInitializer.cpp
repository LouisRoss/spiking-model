#include "ModelLayerInitializer.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using embeddedpenguins::modelengine::sdk::IModelInitializer;
    
    ModelLayerInitializer::ModelLayerInitializer(vector<NeuronNode>& model, json& configuration) :
        ModelNeuronInitializer(model, configuration)
    {
    }

    void ModelLayerInitializer::Initialize()
    {
        model_.resize(configuration_["Model"]["ModelSize"]);

        strength_ = 101;
        InitializeAnInput(0, 1);
        InitializeAnInput(0, 2);
        InitializeAnInput(0, 3);
        InitializeAnInput(0, 4);
        InitializeAnInput(0, 5);

        strength_ = configuration_["Model"]["InitialSynapticStrength"];
        for (auto row = 0; row < height_ - 1; row++)
        {
            InitializeARow(row, row + 1);
        }
        InitializeARow(height_ - 1, 0);
    }

    void ModelLayerInitializer::InjectSignal(ProcessCallback<NeuronOperation, NeuronRecord>& callback)
    {
            for (int i = 1; i < 6; i++)
            {
                callback(NeuronOperation(1, Operation::Spike, 0), (i*3)+0);
                callback(NeuronOperation(2, Operation::Spike, 0), (i*3)-1);
                callback(NeuronOperation(3, Operation::Spike, 0), (i*3)-2);
                callback(NeuronOperation(4, Operation::Spike, 0), (i*3)-3);
                callback(NeuronOperation(5, Operation::Spike, 0), (i*3)-4);
            }
    }

    void ModelLayerInitializer::InitializeARow(int row, int destRow)
    {
        for (auto column = 0; column < width_; column++)
            for (auto destCol = 0; destCol < width_; destCol++)
                InitializeAConnection(row, column, destRow, destCol);
    }

    // the class factories

    extern "C" IModelInitializer<NeuronOperation, NeuronRecord>* create(vector<NeuronNode>& model, json& configuration) {
        return new ModelLayerInitializer(model, configuration);
    }

    extern "C" void destroy(IModelInitializer<NeuronOperation, NeuronRecord>* p) {
        delete p;
    }
}
