#include "Initializers/ModelLayerInitializer.h"
#include "CpuModelCarrier.h"
#include "NeuronModelHelper.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using embeddedpenguins::core::neuron::model::IModelInitializer;
    using embeddedpenguins::core::neuron::model::ModelLayerInitializer;
    
    // the class factories

    extern "C" IModelInitializer<NeuronModelHelper<CpuModelCarrier>>* create(NeuronModelHelper<CpuModelCarrier>& helper) {
        return new ModelLayerInitializer<NeuronModelHelper<CpuModelCarrier>>(helper);
    }

    extern "C" void destroy(IModelInitializer<NeuronModelHelper<CpuModelCarrier>>* p) {
        delete p;
    }
}
