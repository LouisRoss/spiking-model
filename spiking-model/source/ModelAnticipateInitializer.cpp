#include "ModelAnticipateInitializer.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using embeddedpenguins::modelengine::sdk::IModelInitializer;
    
    ModelAnticipateInitializer::ModelAnticipateInitializer(vector<NeuronNode>& model, json& configuration) :
        ModelNeuronInitializer(model, configuration)
    {
    }

    void ModelAnticipateInitializer::Initialize()
    {
        model_.resize(configuration_["Model"]["ModelSize"]);

        strength_ = 51;
        InitializeAConnection(0, 20, 10, 20);
        InitializeAConnection(10, 21, 10, 20);
        strength_ = 58;
        InitializeAConnection(0, 21, 10, 21);
        InitializeAConnection(10, 20, 10, 21);

        model_[20].Synapses[1].IsUsed = true;
        model_[20].Synapses[1].Strength = 101;
        model_[21].Synapses[2].IsUsed = true;
        model_[21].Synapses[2].Strength = 101;
    }

    void ModelAnticipateInitializer::InjectSignal(ProcessCallback<NeuronOperation, NeuronRecord>& callback)
    {
        auto s1 = 0 * 50 + 20;
        auto s2 = 0 * 50 + 21;
        auto d1 = 10 * 50 + 20;

        for (int i = 0; i < 1600; i += 200)
        {
            callback(NeuronOperation(s1, Operation::Spike, 1), i);
            callback(NeuronOperation(s1, Operation::Spike, 1), i+20);
            callback(NeuronOperation(s1, Operation::Spike, 1), i+40);
            callback(NeuronOperation(s2, Operation::Spike, 2), i+60);
        }
    }

    // the class factories

    extern "C" IModelInitializer<NeuronOperation, NeuronRecord>* create(vector<NeuronNode>& model, json& configuration) {
        return new ModelAnticipateInitializer(model, configuration);
    }

    extern "C" void destroy(IModelInitializer<NeuronOperation, NeuronRecord>* p) {
        delete p;
    }
}
