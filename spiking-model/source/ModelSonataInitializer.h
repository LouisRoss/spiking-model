#include <memory>

#include "ModelNeuronInitializer.h"
#include "sdk/IModelPersister.h"
#include "persistence/sonata/SonataModelRepository.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using std::unique_ptr;
    using std::vector;

    using nlohmann::json;

    using embeddedpenguins::modelengine::ModelEngine;

    using embeddedpenguins::neuron::infrastructure::ModelNeuronInitializer;
    using embeddedpenguins::neuron::infrastructure::NeuronNode;
    using embeddedpenguins::neuron::infrastructure::NeuronOperation;
    using embeddedpenguins::neuron::infrastructure::NeuronImplementation;
    using embeddedpenguins::neuron::infrastructure::NeuronRecord;
    using embeddedpenguins::neuron::infrastructure::persistence::IModelPersister;
    using embeddedpenguins::neuron::infrastructure::persistence::sonata::SonataModelRepository;

    //
    // This custom initializer sets up a spiking neuron model as 
    // described by a Sonata-formatted file collection.
    //
    class ModelSonataInitializer : public ModelNeuronInitializer
    {
        bool valid_ { false };
        unique_ptr<IModelPersister<NeuronNode>> persister_ {nullptr};
        unique_ptr<SonataModelRepository> sonataRepository_ {nullptr};

    public:
        ModelSonataInitializer(CpuModelCarrier model, json& configuration);
        virtual void Initialize() override;
        virtual void InjectSignal(ProcessCallback<NeuronOperation, NeuronRecord>& callback) override;
    };
}
