#include "ModelSonataInitializer.h"
#include "persistence/sonata/SonataModelPersister.h"
#include "persistence/sonata/SonataInputSpikeLoader.h"

//#define TESTING
namespace embeddedpenguins::neuron::infrastructure
{
    using std::make_unique;

    using embeddedpenguins::modelengine::sdk::IModelInitializer;

    using embeddedpenguins::neuron::infrastructure::persistence::sonata::SonataModelPersister;
    using embeddedpenguins::neuron::infrastructure::persistence::sonata::SonataInputSpikeLoader;
    
    ModelSonataInitializer::ModelSonataInitializer(vector<NeuronNode>& model, json& configuration) :
        ModelNeuronInitializer(model, configuration)
    {
        cout << "ModelSonataInitializer ctor\n";
        auto sonataConfigurationJson = configuration_["Model"]["SonataConfiguration"];
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

    void ModelSonataInitializer::Initialize()
    {
        if (!valid_)
        {
            cout << "ModelSonataInitializer is not in a valid state and can not initialize\n";
            return;
        }
#ifndef TESTING
        persister_->LoadConfiguration();
        persister_->ReadModel(helper_.Model(), configuration_);
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

    void ModelSonataInitializer::InjectSignal(ProcessCallback<NeuronOperation, NeuronRecord>& callback)
    {
#ifndef TESTING
        SonataInputSpikeLoader spikeLoader(*sonataRepository_);
        spikeLoader.LoadSpikes(500);  // Is this number available in the LGN part of the configuration?
        spikeLoader.PersistSpikes("spikes.csv");

        auto& spikeTimesAndNodes = spikeLoader.Spikes();
        for (auto [spikeTime, spikeNode] : spikeTimesAndNodes)
        {
            //if (spikeNode == 47)
            //{
            //    cout << "Initializing neuron 47 to spike at time " << (int)spikeTime << '\n';
            //}
            callback(NeuronOperation(spikeNode, Operation::Spike, 0), (int)spikeTime + 1000);
        }
#else
       for (auto i = 0; i < 3000; i += 250)
       {
            callback(NeuronOperation(47, Operation::Spike, 0), i);
            callback(NeuronOperation(285, Operation::Spike, 0), i);
            callback(NeuronOperation(312, Operation::Spike, 0), i);
            callback(NeuronOperation(471, Operation::Spike, 0), i);
       }
#endif
    }


    // the class factories

    extern "C" IModelInitializer<NeuronOperation, NeuronRecord>* create(vector<NeuronNode>& model, json& configuration) {
        return new ModelSonataInitializer(model, configuration);
    }

    extern "C" void destroy(IModelInitializer<NeuronOperation, NeuronRecord>* p) {
        delete p;
    }
}
