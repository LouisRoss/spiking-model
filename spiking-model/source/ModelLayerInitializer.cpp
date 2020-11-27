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
        strength_ = configuration_["Model"]["InitialSynapticStrength"];

        for (auto row = 0; row < height_ - 1; row++)
        {
            InitializeARow(row, row + 1);
        }
        InitializeARow(height_ - 1, 0);
    }

    void ModelLayerInitializer::InjectSignal(ModelEngine<NeuronNode, NeuronOperation, NeuronImplementation, NeuronRecord>& modelEngine)
    {
            for (int i = 5; i; i--)
            {
                modelEngine.QueueWork(NeuronOperation(1, Operation::Spike, i), (i*3)+0);
                modelEngine.QueueWork(NeuronOperation(2, Operation::Spike, i), (i*3)-1);
                modelEngine.QueueWork(NeuronOperation(3, Operation::Spike, i), (i*3)-2);
                modelEngine.QueueWork(NeuronOperation(4, Operation::Spike, i), (i*3)-3);
                modelEngine.QueueWork(NeuronOperation(5, Operation::Spike, i), (i*3)-4);
            }
    }

    void ModelLayerInitializer::InitializeARow(int row, int destRow)
    {
        for (auto column = 0; column < width_; column++)
            for (auto destCol = 0; destCol < width_; destCol++)
                InitializeAConnection(row, column, destRow, destCol);
    }

    // the class factories

    extern "C" IModelInitializer<NeuronNode, NeuronOperation, NeuronImplementation, NeuronRecord>* create(vector<NeuronNode>& model, json& configuration) {
        return new ModelLayerInitializer(model, configuration);
    }

    extern "C" void destroy(IModelInitializer<NeuronNode, NeuronOperation, NeuronImplementation, NeuronRecord>* p) {
        delete p;
    }
}
