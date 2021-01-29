#pragma once

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

#include "NeuronNode.h"
#include "SensorInput/SensorInputProxy.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using std::string;
    using std::vector;

    using nlohmann::json;

    using embeddedpenguins::neuron::infrastructure::sensorinput::SensorInputProxy;

    struct CpuModelCarrier
    {
        vector<NeuronNode>& Model;
        SensorInputProxy sensorInput_;
        unsigned long int ModelSize() { return Model.size(); }

        CpuModelCarrier(vector<NeuronNode>& model, const string& sensorInputSharedLibraryPath, const json& configuration) : 
            Model(model), 
            sensorInput_(sensorInputSharedLibraryPath)
        {
            sensorInput_.CreateProxy(configuration);
        }

        CpuModelCarrier(const CpuModelCarrier& other) = delete;
        const CpuModelCarrier operator=(const CpuModelCarrier& other) = delete;
    };
}
