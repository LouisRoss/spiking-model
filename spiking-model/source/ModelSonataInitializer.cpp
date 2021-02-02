#include "ModelSonataInitializer.h"
#include "CpuModelCarrier.h"
#include "NeuronModelHelper.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using embeddedpenguins::modelengine::sdk::IModelInitializer;

    // the class factories

    extern "C" IModelInitializer<NeuronOperation, NeuronRecord>* create(CpuModelCarrier carrier, ConfigurationUtilities& configuration) {
        return new ModelSonataInitializer<NeuronModelHelper<CpuModelCarrier>>(configuration, NeuronModelHelper<CpuModelCarrier>(carrier, configuration));
    }

    extern "C" void destroy(IModelInitializer<NeuronOperation, NeuronRecord>* p) {
        delete p;
    }
}
