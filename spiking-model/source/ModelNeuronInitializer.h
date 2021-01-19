#pragma once

#include <vector>
#include <chrono>

#include "nlohmann/json.hpp"

#include "ModelEngine.h"
#include "sdk/ModelInitializer.h"

#include "NeuronNode.h"
#include "NeuronOperation.h"
#include "NeuronImplementation.h"
#include "NeuronRecord.h"
#include "NeuronModelHelper.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;
    using std::chrono::hours;
    using std::chrono::duration_cast;
    using nlohmann::json;
    using embeddedpenguins::modelengine::ModelEngine;
    using embeddedpenguins::modelengine::sdk::ModelInitializer;

    //
    // Intermediate base class for models implementing neuron dynamics.
    //
    class ModelNeuronInitializer : public ModelInitializer<NeuronNode, NeuronOperation, NeuronRecord>
    {
    public:
        struct Neuron2Dim
        {
            unsigned long long int Row {};
            unsigned long long int Column {};
        };

    protected:
        NeuronModelHelper helper_;

        int width_ { 50 };
        int height_ { 25 };
        int strength_ { 21 };

    public:
        ModelNeuronInitializer(vector<NeuronNode>& model, json& configuration) :
            ModelInitializer(configuration),
            helper_(model, configuration)
        {
        }

    protected:
        void InitializeAnInput(int row, int column)
        {
            auto sourceIndex = GetIndex(row, column);
            helper_.WireInput(sourceIndex, strength_);
        }

        void InitializeAnInput(const Neuron2Dim& neuron)
        {
            InitializeAnInput(neuron.Row, neuron.Column);
        }

        void InitializeAConnection(const int row, const int column, const int destRow, const int destCol)
        {
            auto sourceIndex = GetIndex(row, column);
            auto destinationIndex = GetIndex(destRow, destCol);
            helper_.Wire(sourceIndex, destinationIndex, strength_);
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

        NeuronType GetNeuronType(int row, int column) const
        {
            return helper_.GetNeuronType(GetIndex(row, column));
        }

        NeuronType GetNeuronType(const Neuron2Dim& source) const
        {
            return GetNeuronType(source.Row, source.Column);
        }

        void SetNeuronType(int row, int column, NeuronType type)
        {
            return helper_.SetNeuronType(GetIndex(row, column), type);
        }

        void SetNeuronType(const Neuron2Dim& source, NeuronType type)
        {
            return SetNeuronType(source.Row, source.Column, type);
        }
    };
}
