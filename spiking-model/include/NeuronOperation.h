#pragma once

#include "NeuronCommon.h"

namespace embeddedpenguins::neuron::infrastructure
{
    struct NeuronOperation
    {
        unsigned long long int Index {0};
        long int Data {0};
        Operation Op;

        NeuronOperation() : Op(Operation::Decay) { }
        NeuronOperation(unsigned long long int index, Operation op, long int data = 0L) : Index(index), Op(op), Data(data) { }
    };
}
