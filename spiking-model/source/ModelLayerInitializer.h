#include "ModelNeuronInitializer.h"
#include "nlohmann/json.hpp"

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
    // the 'layer' test, which demonstrates a continuous wave action
    // in layers.
    //
    class ModelLayerInitializer : public ModelNeuronInitializer
    {
    public:
        ModelLayerInitializer(vector<NeuronNode>& model, json& configuration);
        virtual void Initialize() override;
        virtual void InjectSignal(ModelEngine<NeuronNode, NeuronOperation, NeuronImplementation, NeuronRecord>& modelEngine) override;

    private:
        void InitializeARow(int row, int destRow);
    };
}
