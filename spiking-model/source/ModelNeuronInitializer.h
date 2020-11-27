#pragma once

#include <vector>
#include <chrono>
#include "ModelEngine.h"
#include "nlohmann/json.hpp"
#include "sdk/ModelInitializer.h"
#include "NeuronModelHelper.h"
#include "NeuronNode.h"
#include "NeuronOperation.h"
#include "NeuronImplementation.h"
#include "NeuronRecord.h"

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
    class ModelNeuronInitializer : public ModelInitializer<NeuronNode, NeuronOperation, NeuronImplementation, NeuronRecord>
    {
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
            auto duration_since_epoch = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
            duration_since_epoch -= duration_cast<milliseconds>(hours(12));
            auto milliseconds_since_epoch = duration_since_epoch.count();
            unsigned int timestamp = static_cast<unsigned int>(milliseconds_since_epoch);

            for (auto& neuron : model_)
            {
                neuron.TickLastSpike = timestamp;

                for (auto& synapse : neuron.Synapses)
                {
                    synapse.Strength = 0;
                    synapse.IsUsed = false;
                    synapse.TickLastSignal = timestamp;
                }
                for (auto& connection : neuron.PostsynapticConnections)
                {
                    connection.PostsynapticNeuron = -1LL;
                    connection.Synapse = -1;
                }
            }
        }

    protected:
        void InitializeAConnection(int row, int column, int destRow, int destCol)
        {
            auto sourceIndex = GetIndex(row, column);
            auto destinationIndex = GetIndex(destRow, destCol);
            helper_.Wire(sourceIndex, destinationIndex, strength_);
        }

        unsigned long long int GetIndex(int row, int column)
        {
            return row * width_ + column;
        }

        NeuronNode& GetAt(int row, int column)
        {
            return model_[GetIndex(row, column)];
        }
    };
}
