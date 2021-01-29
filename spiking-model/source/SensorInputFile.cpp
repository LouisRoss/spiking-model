#include "SensorInputFile.h"

namespace embeddedpenguins::neuron::infrastructure::sensorinput
{
    // the class factories

    extern "C" ISensorInput* create(const json& configuration) {
        return new SensorInputFile(configuration);
    }

    extern "C" void destroy(ISensorInput* p) {
        delete p;
    }
}
