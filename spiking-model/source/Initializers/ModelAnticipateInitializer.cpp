#include "NeuronModelHelper.h"
#include "CpuModelCarrier.h"

#include "Initializers/ModelAnticipateInitializer.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using embeddedpenguins::core::neuron::model::IModelInitializer;
    using embeddedpenguins::core::neuron::model::ModelAnticipateInitializer;
    
    // the class factories

    extern "C" IModelInitializer<NeuronModelHelper<CpuModelCarrier>>* create(NeuronModelHelper<CpuModelCarrier>& helper) {
        return new ModelAnticipateInitializer<NeuronModelHelper<CpuModelCarrier>>(helper);
    }

    extern "C" void destroy(IModelInitializer<NeuronModelHelper<CpuModelCarrier>>* p) {
        delete p;
    }
}
