#include <memory>
#include <string>

#include "ModelEngineCommon.h"

#include "ModelNeuronInitializer.h"
#include "sdk/IModelPersister.h"
#include "persistence/sonata/SonataModelRepository.h"
#include "persistence/sonata/SonataModelPersister.h"
#include "persistence/sonata/SonataInputSpikeLoader.h"
#include "CpuModelCarrier.h"

//#define TESTING
namespace embeddedpenguins::neuron::infrastructure
{
    using std::unique_ptr;
    using std::make_unique;
    using std::string;

    using nlohmann::json;

    using embeddedpenguins::modelengine::ConfigurationUtilities;
    using embeddedpenguins::neuron::infrastructure::ModelNeuronInitializer;
    using embeddedpenguins::neuron::infrastructure::NeuronOperation;
    using embeddedpenguins::neuron::infrastructure::NeuronRecord;
    using embeddedpenguins::neuron::infrastructure::CpuModelCarrier;
    using embeddedpenguins::modelengine::threads::ProcessCallback;
    using embeddedpenguins::neuron::infrastructure::persistence::IModelPersister;
    using embeddedpenguins::neuron::infrastructure::persistence::sonata::SonataModelRepository;
    using embeddedpenguins::neuron::infrastructure::persistence::sonata::SonataModelPersister;
    using embeddedpenguins::neuron::infrastructure::persistence::sonata::SonataInputSpikeLoader;

    //
    // This custom initializer sets up a spiking neuron model as 
    // described by a Sonata-formatted file collection.
    //
    template<class MODELHELPERTYPE>
    class ModelSonataInitializer : public ModelNeuronInitializer<MODELHELPERTYPE>
    {
        bool valid_ { false };
        unique_ptr<IModelPersister<CpuModelCarrier>> persister_ {nullptr};
        unique_ptr<SonataModelRepository> sonataRepository_ {nullptr};

    public:
        ModelSonataInitializer(ConfigurationUtilities& configuration, MODELHELPERTYPE helper)  :
            ModelNeuronInitializer<MODELHELPERTYPE>(configuration, helper)
        {
            cout << "ModelSonataInitializer ctor\n";
            const json& sonataConfigurationJson = this->configuration_.Configuration()["Model"]["SonataConfiguration"];
            if (!sonataConfigurationJson.is_string())
            {
                cout << "ModelSonataInitializer unable to find sonata configuration file path\n";
                return;
            }
            auto sonataConfiguration = sonataConfigurationJson.get<string>();

            sonataRepository_ = make_unique<SonataModelRepository>(sonataConfiguration);
            persister_ = make_unique<SonataModelPersister>(sonataConfiguration, *sonataRepository_);
            valid_ = true;
        }

        virtual void Initialize() override
        {
            if (!valid_)
            {
                cout << "ModelSonataInitializer is not in a valid state and can not initialize\n";
                return;
            }
#ifndef TESTING
            persister_->LoadConfiguration();
            persister_->ReadModel(this->helper_.Carrier(), this->configuration_);
#else
            helper_.InitializeModel(900);

            unsigned long int sourceNodes[] = {47, 285, 312, 471};
            unsigned long int targetNodes[] = {651, 685, 702};

            for (auto sourceNode : sourceNodes)
            {
                helper_.WireInput(sourceNode, 101);

                for (auto targetNode : targetNodes)
                {
                    helper_.Wire(sourceNode, targetNode, 35);
                }
            }
#endif
        }
    };
}
