#pragma once

#include <vector>

#include "NeuronNode.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using std::vector;

    struct CpuModelCarrier
    {
        vector<NeuronNode>& Model;
        unsigned long int ModelSize() { return Model.size(); }
    };
}
