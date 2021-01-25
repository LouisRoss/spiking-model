#include "ModelSonataInitializer.h"
#include "CpuModelCarrier.h"
#include "NeuronModelHelper.h"

namespace embeddedpenguins::neuron::infrastructure
{
    //using std::make_unique;

    using embeddedpenguins::modelengine::sdk::IModelInitializer;

    //using embeddedpenguins::neuron::infrastructure::persistence::sonata::SonataModelPersister;
    //using embeddedpenguins::neuron::infrastructure::persistence::sonata::SonataInputSpikeLoader;
    
    // the class factories

    extern "C" IModelInitializer<NeuronOperation, NeuronRecord>* create(CpuModelCarrier carrier, json& configuration) {
        return new ModelSonataInitializer<NeuronModelHelper<CpuModelCarrier>>(configuration, NeuronModelHelper<CpuModelCarrier>(carrier, configuration));
    }

    extern "C" void destroy(IModelInitializer<NeuronOperation, NeuronRecord>* p) {
        delete p;
    }
}
