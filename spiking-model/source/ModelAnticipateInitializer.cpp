#include "NeuronModelHelper.h"
#include "CpuModelCarrier.h"
#include "ModelAnticipateInitializer.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using embeddedpenguins::modelengine::sdk::IModelInitializer;
    
    // the class factories

    extern "C" IModelInitializer<NeuronOperation, NeuronRecord>* create(CpuModelCarrier& carrier, json& configuration) {
        return new ModelAnticipateInitializer<NeuronModelHelper<CpuModelCarrier>>(configuration, NeuronModelHelper<CpuModelCarrier>(carrier, configuration));
    }

    extern "C" void destroy(IModelInitializer<NeuronOperation, NeuronRecord>* p) {
        delete p;
    }
}
