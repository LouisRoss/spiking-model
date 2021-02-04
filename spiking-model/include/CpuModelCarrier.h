#pragma once

#include <string>
#include <vector>

#include "NeuronNode.h"
#include "ModelEngineCommon.h"
#include "SensorInput/SensorInputProxy.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using std::string;
    using std::vector;

    using embeddedpenguins::modelengine::ConfigurationUtilities;
    using embeddedpenguins::neuron::infrastructure::sensorinput::SensorInputProxy;

    struct CpuModelCarrier
    {
        vector<NeuronNode>& Model;
        SensorInputProxy SensorInput;
        unsigned long int ModelSize() { return Model.size(); }

        CpuModelCarrier(vector<NeuronNode>& model, const string& sensorInputSharedLibraryPath) : 
            Model(model), 
            SensorInput(sensorInputSharedLibraryPath)
        {
        }

        CpuModelCarrier(const CpuModelCarrier& other) = delete;
        const CpuModelCarrier operator=(const CpuModelCarrier& other) = delete;
    };
}
