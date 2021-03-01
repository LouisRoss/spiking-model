#include "Initializers/ModelSonataInitializer.h"
#include "CpuModelCarrier.h"
#include "NeuronModelHelper.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using embeddedpenguins::core::neuron::model::IModelInitializer;
    using embeddedpenguins::core::neuron::model::ModelSonataInitializer;

    // the class factories

    extern "C" IModelInitializer<NeuronModelHelper<CpuModelCarrier>>* create(NeuronModelHelper<CpuModelCarrier>& helper) {
        return new ModelSonataInitializer<NeuronModelHelper<CpuModelCarrier>>(helper);
    }

    extern "C" void destroy(IModelInitializer<NeuronModelHelper<CpuModelCarrier>>* p) {
        delete p;
    }
}
