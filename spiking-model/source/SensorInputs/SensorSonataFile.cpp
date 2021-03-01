#include "ModelEngineCommon.h"
#include "SensorSonataFile.h"

using ::embeddedpenguins::modelengine::ConfigurationUtilities;

namespace embeddedpenguins::neuron::infrastructure::sensorinput
{
    // the class factories

    extern "C" ISensorInput* create(const ConfigurationUtilities& configuration) {
        return new SensorSonataFile(configuration);
    }

    extern "C" void destroy(ISensorInput* p) {
        delete p;
    }
}
