#pragma once

#include <iostream>
#include <vector>
#include <cctype>
#include <algorithm>
#include <cmath>

#include "NeuronModelHelper.h"
#include "NeuronNode.h"

#include "SonataNodeModel.h"
#include "SonataCompiledModel.h"

namespace embeddedpenguins::neuron::infrastructure::persistence::sonata
{
    using std::cout;
    using std::vector;
    using std::tolower;
    using std::transform;
    using std::log10;
    using embeddedpenguins::neuron::infrastructure::NeuronModelHelper;
    using embeddedpenguins::neuron::infrastructure::NeuronType;
    using embeddedpenguins::neuron::infrastructure::NeuronNode;

    class SonataModelLoader
    {
        bool isValid_ { false };
        SonataPopulationCollection& populations_;
        const SonataConnections& modelConnections_;

    public:
        const bool IsValid() const { return isValid_; }

    public:
        SonataModelLoader(SonataPopulationCollection& populations, const SonataConnections& modelConnections) :
            populations_(populations),
            modelConnections_(modelConnections)
        {
            
        }

        void LoadModel(vector<NeuronNode>& model)
        {
            NeuronModelHelper helper(model);

            InitializeModelNodes(model);
            WireModelConnections(helper);

            auto [postsynapticConnections, presynapticConnections] = helper.FindRequiredSynapseCounts();
        }

    private:
        void InitializeModelNodes(vector<NeuronNode>& model)
        {
            auto nodeCount = populations_.NodeCount();
            model.resize(nodeCount);
            cout << "Setting model size to " << nodeCount << " nodes\n";

            auto modelIndex { 0UL };
            for (auto& population : populations_)
            {
                for (auto& nodeGroup : population)
                {
                    for (auto i = 0; i < nodeGroup.NodeCount(); i++)
                    {
                        model[modelIndex].Type = nodeGroup.Action() == NodeAction::Excitatory ? NeuronType::Excitatory : NeuronType::Inhibitory;
                        modelIndex++;
                    }
                }
            }

            populations_.SetPopulationNodeOffsets();
            for (const auto& population : populations_)
            {
                cout << "Node offset for population " << population.PopulationName() << "(" << population.NodeCount() << " nodes) set to " << population.NodeOffset() << '\n';
            }
        }

        void WireModelConnections(NeuronModelHelper& helper)
        {
            static int LowestBiophysicalWeight {1000};
            static int HighestBiophysicalWeight {0};
            static int LowestPointProcessWeight {1000};
            static int HighestPointProcessWeight {0};
            
            unsigned long int connectionCount {0};
            for (const auto& connection : modelConnections_)
            {
                auto [modelSourceNodeIndex, sourceModelTypeLc] = GetConnectionSourceDetails(connection);
                if (sourceModelTypeLc == "virtual")
                {
                    helper.WireInput(modelSourceNodeIndex, 101);
                }

                auto [modelTargetNodeIndex, targetModelTypeLc] = GetConnectionTargetDetails(connection);
                auto calibratedSynapticWeight = CalibrateSynapticWeight(targetModelTypeLc, connection.SynapticWeight());
                if (targetModelTypeLc == "biophysical")
                {
                    if (calibratedSynapticWeight < LowestBiophysicalWeight) LowestBiophysicalWeight = calibratedSynapticWeight;
                    if (calibratedSynapticWeight > HighestBiophysicalWeight) HighestBiophysicalWeight = calibratedSynapticWeight;
                }
                else
                {
                    if (calibratedSynapticWeight < LowestPointProcessWeight) LowestPointProcessWeight = calibratedSynapticWeight;
                    if (calibratedSynapticWeight > HighestPointProcessWeight) HighestPointProcessWeight = calibratedSynapticWeight;
                }
                

                for (auto _ = connection.NumberOfSynapses(); _; --_)
                {
                    helper.Wire(modelSourceNodeIndex, modelTargetNodeIndex, calibratedSynapticWeight);
                    connectionCount++;
                }
            }

            cout << "Wired " << connectionCount << " connections: [biophysical: L = " << LowestBiophysicalWeight << ", H = " << HighestBiophysicalWeight << "] [point process L = " << LowestPointProcessWeight << ", H = " << HighestPointProcessWeight << "]\n";
        }

        std::tuple<unsigned long int, string> GetConnectionSourceDetails(const SonataFromTo& connection)
        {
            const auto& sourcePopulationName = connection.SourcePopulation();
            auto& sourcePopulation = populations_.FindPopulation(sourcePopulationName);
            auto sourceBaseOffset = sourcePopulation.NodeOffset();
            auto sourceNodeIndex = connection.SourceNodeId();
            auto modelSourceNodeIndex = sourceBaseOffset + sourceNodeIndex;
            auto sourceNodeGroup = sourcePopulation.GetFromIndex(sourceNodeIndex);

            auto sourceModelTypeLc{sourceNodeGroup.ModelType()};
            transform(sourceModelTypeLc.begin(), sourceModelTypeLc.end(), sourceModelTypeLc.begin(), [](unsigned char c){ return tolower(c); });

            return std::make_tuple(modelSourceNodeIndex, sourceModelTypeLc);
        }

        std::tuple<unsigned long int, string> GetConnectionTargetDetails(const SonataFromTo& connection)
        {
            const auto& targetPopulationName = connection.TargetPopulation();
            auto& targetPopulation = populations_.FindPopulation(targetPopulationName);
            auto targetBaseOffset = targetPopulation.NodeOffset();
            auto targetNodeIndex = connection.TargetNodeId();
            auto modelTargetNodeIndex = targetBaseOffset + targetNodeIndex;
            auto targetNodeGroup = targetPopulation.GetFromIndex(targetNodeIndex);

            auto targetModelTypeLc{targetNodeGroup.ModelType()};
            transform(targetModelTypeLc.begin(), targetModelTypeLc.end(), targetModelTypeLc.begin(), [](unsigned char c){ return tolower(c); });

            return std::make_tuple(modelTargetNodeIndex, targetModelTypeLc);
        }

        int CalibrateSynapticWeight(const string& modelType, double synapticWeight)
        {
            double scaleFactor = 1.0;
            if (modelType == "biophysical")
            {
                scaleFactor = 20000.0;
            }
            else if (modelType == "point_process")
            {
                scaleFactor = 1500.0;
            }

            auto scaledSynapticWeight = (int)(10.0 * log10(synapticWeight * scaleFactor));
            if (scaledSynapticWeight == 0)
                cout << "Configured syaptic weight " << synapticWeight << " for model type " << modelType << ", scaled by " << scaleFactor << " results in scaled weight " << scaledSynapticWeight << '\n';

            return scaledSynapticWeight;
        }
    };
}
