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

    private:
        NeuronModelHelper helper_;

    protected:
        int width_ { 50 };
        int height_ { 25 };
        int strength_ { 21 };

    public:
        ModelNeuronInitializer(vector<NeuronNode>& model, json& configuration) :
            ModelInitializer(model, configuration),
            helper_(model)
        {
            // Get a timestamp that is about 12 hours in the past to initialize with.
            /*
            auto duration_since_epoch = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
            duration_since_epoch -= duration_cast<milliseconds>(hours(12));
            auto milliseconds_since_epoch = duration_since_epoch.count();
            unsigned int timestamp = static_cast<unsigned int>(milliseconds_since_epoch);
            */
            auto longTimeInThePast = numeric_limits<unsigned long long int>::max() - 1000ULL;

            for (auto& neuron : model_)
            {
                neuron.TickLastSpike = longTimeInThePast;

                for (auto& synapse : neuron.Synapses)
                {
                    synapse.Strength = 0;
                    synapse.IsUsed = false;
                    synapse.TickLastSignal = longTimeInThePast;
                }
                for (auto& connection : neuron.PostsynapticConnections)
                {
                    connection.PostsynapticNeuron = -1LL;
                    connection.Synapse = -1;
                }
            }
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

        void InitializeAConnection(int row, int column, int destRow, int destCol)
        {
            auto sourceIndex = GetIndex(row, column);
            auto destinationIndex = GetIndex(destRow, destCol);
            helper_.Wire(sourceIndex, destinationIndex, strength_);
        }

        void InitializeAConnection(const Neuron2Dim& source, const Neuron2Dim& destination)
        {
            InitializeAConnection(source.Row, source.Column, destination.Row, destination.Column);
        }

        unsigned long long int GetIndex(int row, int column)
        {
            return row * width_ + column;
        }

        unsigned long long int GetIndex(const Neuron2Dim& source)
        {
            return GetIndex(source.Row, source.Column);
        }

        NeuronNode& GetAt(int row, int column)
        {
            return model_[GetIndex(row, column)];
        }

        NeuronNode& GetAt(const Neuron2Dim& source)
        {
            return GetAt(source.Row, source.Column);
        }
    };
}
