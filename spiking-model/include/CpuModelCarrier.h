#pragma once

#include <string>
#include <vector>

#include "NeuronNode.h"
#include "sdk/SensorInputProxy.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using std::string;
    using std::vector;

    using embeddedpenguins::core::neuron::model::SensorInputProxy;

    struct CpuModelCarrier
    {
        vector<NeuronNode>& Model;
        SensorInputProxy SensorInput;
        unsigned long int ModelSize() { return Model.size(); }
        bool Valid { true };

        CpuModelCarrier(vector<NeuronNode>& model, const string& sensorInputSharedLibraryPath) : 
            Model(model), 
            SensorInput(sensorInputSharedLibraryPath)
        {
        }

        CpuModelCarrier(const CpuModelCarrier& other) = delete;
        const CpuModelCarrier operator=(const CpuModelCarrier& other) = delete;
    };
}
