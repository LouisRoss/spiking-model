#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <limits>

#include "nlohmann/json.hpp"

#include "ModelEngineCommon.h"
#include "sdk/ModelInitializer.h"

#include "NeuronOperation.h"
#include "NeuronRecord.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using std::cout;
    using std::string;
    using std::vector;
    using std::map;
    using std::tuple;
    using std::make_tuple;
    using std::numeric_limits;

    using nlohmann::json;

    using embeddedpenguins::modelengine::ConfigurationUtilities;
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
        unsigned long long int maxIndex_ { };
        int strength_ { 21 };

        map<string, tuple<int, int>> namedNeurons_ { };

    public:
    public:
        ModelNeuronInitializer(ConfigurationUtilities& configuration, MODELHELPERTYPE helper) :
            ModelInitializer<NeuronOperation, MODELHELPERTYPE, NeuronRecord>(configuration, helper)
        {
            LoadOptionalDimensions();
            LoadOptionalNamedNeurons();
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

        const Neuron2Dim ResolveNeuron(const string& neuronName) const
        {
            auto neuronIt = namedNeurons_.find(neuronName);
            if (neuronIt != namedNeurons_.end())
            {
                auto& [row, col] = neuronIt->second;
                Neuron2Dim yyy { .Row = (unsigned long long)row, .Column = (unsigned long long)col };
                cout << "ResolveNeuron(" << neuronName << ") found coordinates [" << yyy.Row << ", " << yyy.Column << "]\n";
                return yyy;
            }

            Neuron2Dim xxx { .Row = numeric_limits<unsigned long long>::max(), .Column = numeric_limits<unsigned long long>::max() };
            cout << "ResolveNeuron(" << neuronName << ") NOT found coordinates [" << xxx.Row << ", " << xxx.Column << "]\n";
            return xxx;
        }

    private:
        void LoadOptionalDimensions()
        {
            // Override the dimension defaults if configured.
            const json& configuration = this->configuration_.Configuration();
            auto& modelSection = configuration["Model"];
            if (!modelSection.is_null() && modelSection.contains("Dimensions"))
            {
                auto dimensionElement = modelSection["Dimensions"];
                if (dimensionElement.is_array())
                {
                    auto dimensionArray = dimensionElement.get<std::vector<int>>();
                    width_ = dimensionArray[0];
                    height_ = dimensionArray[1];
                }
            }

            maxIndex_ = width_ * height_;
        }

        void LoadOptionalNamedNeurons()
        {
            const json& configuration = this->configuration_.Configuration();
            auto& modelSection = configuration["Model"];
            if (!modelSection.is_null() && modelSection.contains("Neurons"))
            {
                auto& namedNeuronsElement = modelSection["Neurons"];
                if (namedNeuronsElement.is_object())
                {
                    for (auto& neuron: namedNeuronsElement.items())
                    {
                        auto neuronName = neuron.key();
                        auto positionArray = neuron.value().get<std::vector<int>>();
                        auto xpos = positionArray[0];
                        auto ypos = positionArray[1];

                        namedNeurons_[neuronName] = make_tuple(xpos, ypos);
                    }
                }
            }
        }
    };
}
