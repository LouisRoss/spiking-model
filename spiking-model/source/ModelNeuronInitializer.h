#pragma once

#include "nlohmann/json.hpp"

#include "sdk/ModelInitializer.h"

#include "NeuronOperation.h"
#include "NeuronRecord.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using nlohmann::json;
    using embeddedpenguins::modelengine::sdk::ModelInitializer;

    struct Neuron2Dim
    {
        unsigned long long int Row {};
        unsigned long long int Column {};
    };

    //
    // Intermediate base class for models implementing neuron dynamics.
    //
    template<class MODELHELPERTYPE>
    class ModelNeuronInitializer : public ModelInitializer<NeuronOperation, MODELHELPERTYPE, NeuronRecord>
    {
    protected:
        int width_ { 50 };
        int height_ { 25 };
        int strength_ { 21 };

    public:
        ModelNeuronInitializer(json& configuration, MODELHELPERTYPE helper) :
            ModelInitializer<NeuronOperation, MODELHELPERTYPE, NeuronRecord>(configuration, helper)
        {
        }

    protected:
        void InitializeAnInput(int row, int column)
        {
            auto sourceIndex = GetIndex(row, column);
            this->helper_.WireInput(sourceIndex, strength_);
        }

        void InitializeAnInput(const Neuron2Dim& neuron)
        {
            InitializeAnInput(neuron.Row, neuron.Column);
        }

        void InitializeAConnection(const int row, const int column, const int destRow, const int destCol)
        {
            auto sourceIndex = GetIndex(row, column);
            auto destinationIndex = GetIndex(destRow, destCol);
            this->helper_.Wire(sourceIndex, destinationIndex, strength_);
        }

        void InitializeAConnection(const Neuron2Dim& source, const Neuron2Dim& destination)
        {
            InitializeAConnection(source.Row, source.Column, destination.Row, destination.Column);
        }

        unsigned long long int GetIndex(const int row, const int column) const
        {
            return row * width_ + column;
        }

        unsigned long long int GetIndex(const Neuron2Dim& source)
        {
            return GetIndex(source.Row, source.Column);
        }

        void SetExcitatoryNeuronType(const unsigned long long int source)
        {
            this->helper_.SetExcitatoryNeuronType(source);
        }

        void SetExcitatoryNeuronType(const Neuron2Dim& source)
        {
            SetExcitatoryNeuronType(GetIndex(source.Row, source.Column));
        }

        void SetInhibitoryNeuronType(const unsigned long long int source)
        {
            this->helper_.SetInhibitoryNeuronType(source);
        }

        void SetInhibitoryNeuronType(const Neuron2Dim& source)
        {
            SetInhibitoryNeuronType(GetIndex(source.Row, source.Column));
        }
    };
}
