#include "ConfigurationRepository.h"
#include "SensorInputs/SensorSonataFile.h"
#include "CpuModelCarrier.h"

namespace embeddedpenguins::neuron::infrastructure::sensorinput
{
    using embeddedpenguins::core::neuron::model::ConfigurationRepository;
    using embeddedpenguins::core::neuron::model::ISensorInput;
    using embeddedpenguins::core::neuron::model::SensorSonataFile;
    using embeddedpenguins::neuron::infrastructure::CpuModelCarrier;

    // the class factories

    extern "C" ISensorInput* create(const ConfigurationRepository& configuration) {
        return new SensorSonataFile<CpuModelCarrier>(configuration);
    }

    extern "C" void destroy(ISensorInput* p) {
        delete p;
    }
}
