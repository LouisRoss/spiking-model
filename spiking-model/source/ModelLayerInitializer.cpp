#include "ModelLayerInitializer.h"
#include "CpuModelCarrier.h"
#include "NeuronModelHelper.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using embeddedpenguins::modelengine::sdk::IModelInitializer;
    
    // the class factories

    extern "C" IModelInitializer<NeuronOperation, NeuronRecord>* create(CpuModelCarrier carrier, json& configuration) {
        return new ModelLayerInitializer<NeuronModelHelper<CpuModelCarrier>>(configuration, NeuronModelHelper<CpuModelCarrier>(carrier, configuration));
    }

    extern "C" void destroy(IModelInitializer<NeuronOperation, NeuronRecord>* p) {
        delete p;
    }
}
