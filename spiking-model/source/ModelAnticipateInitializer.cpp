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

        strength_ = 101;
        InitializeAConnection(I1, N1);
        strength_ = 51;
        InitializeAConnection(N2, N1);

        strength_ = 101;
        InitializeAConnection(I2, N2);
        strength_ = 51;
        InitializeAConnection(N1, N2);

        strength_ = 101;
        InitializeAnInput(I1);
        InitializeAnInput(I2);

        strength_ = 101;
        auto& inh1 = GetAt(Inh1);
        inh1.Type = NeuronType::Inhibitory;
        InitializeAConnection(N1, Inh1);
        InitializeAConnection(Inh1, I1);

        strength_ = 101;
        auto& inh2 = GetAt(Inh2);
        inh2.Type = NeuronType::Inhibitory;
        InitializeAConnection(N2, Inh2);
        InitializeAConnection(Inh2, I2);
    }

    void ModelAnticipateInitializer::InjectSignal(ProcessCallback<NeuronOperation, NeuronRecord>& callback)
    {
        auto i1Index = GetIndex(I1);
        auto i2Index = GetIndex(I2);

        for (int i = 0; i < 8000; i += 200)
        {
            callback(NeuronOperation(i1Index, Operation::Spike, 0), i);
            callback(NeuronOperation(i2Index, Operation::Spike, 0), i+(SignalDelayTime*4)+2);
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
