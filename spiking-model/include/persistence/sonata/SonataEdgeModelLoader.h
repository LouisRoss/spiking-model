#pragma once

#include <iostream>
#include <regex>
#include <memory>

#include "csv-parser/csv.hpp"
#include "nlohmann/json.hpp"
#include "highfive/H5Easy.hpp"

#include "SonataModelRepository.h"
#include "SonataEdgeModel.h"
#include "SonataCompiledModel.h"

namespace embeddedpenguins::neuron::infrastructure::persistence::sonata
{
    using std::cout;
    using std::regex;
    using std::regex_match;
    using std::unique_ptr;
    using std::make_unique;
    using nlohmann::json;

    class SonataEdgeModelLoader
    {
        bool isValid_ { false };

        const SonataModelRepository& sonataRepository_;
        SonataConnections modelConnections_ {};

    public:
        const bool IsValid() const { return isValid_; }
        const SonataConnections& ModelConnections() const { return modelConnections_; }
        SonataConnections& ModelConnections() { return modelConnections_; }

    public:
        SonataEdgeModelLoader(const SonataModelRepository& repository) :
            sonataRepository_(repository)
        {
        }

        void MapEdges(const json& edges)
        {
            // NOTE - The large containers being used here can be too large
            //        to keep on the stack, so are created on the heap.
            auto sonataEdgesCollection = make_unique<vector<SonataEdges>>();
            for (auto& edge : edges)
            {
                sonataEdgesCollection->push_back(SonataEdges());
                auto& sonataEdges = sonataEdgesCollection->back();
                FillEdgeTypeCommon(edge, sonataEdges);
                FillEdgeData(edge, sonataEdges);
            }

            MapSonataEdgesToModelConnections(*sonataEdgesCollection.get());
        }

    private:
        void FillEdgeTypeCommon(const json& edge, SonataEdges& sonataEdges)
        {
            auto edgetypesFileName = sonataRepository_.GetStringFromConfiguration(edge, NETWORK_NETWORKS_EDGES_TYPES_FILE);
            csv::CSVFormat format;
            format.delimiter(' ');
            csv::CSVReader reader(edgetypesFileName, format);
            for (auto& row : reader)
            {
                auto columnNames = row.get_col_names();

                int typeId {};
                double syn_weight {};
                double delay {};

                if (find_if(begin(columnNames), end(columnNames), [](string cn){ return cn == "edge_type_id"; }) != columnNames.end())
                    typeId = row["edge_type_id"].get<int>();
                if (find_if(begin(columnNames), end(columnNames), [](string cn){ return cn == "syn_weight"; }) != columnNames.end())
                    syn_weight = row["syn_weight"].get<double>();
                if (find_if(begin(columnNames), end(columnNames), [](string cn){ return cn == "delay"; }) != columnNames.end())
                    delay = row["delay"].get<double>();

                sonataEdges.EdgeTypeInfo().insert(std::make_pair(typeId, SonataEdgeTypeCommon(typeId, syn_weight, delay)));
            }

            cout << "Found edge types file " << edgetypesFileName << " and loaded " << sonataEdges.EdgeTypeInfo().size() << " edge types\n";
        }

        void FillEdgeData(const json& edge, SonataEdges& sonataEdges)
        {
            auto edgesFileName = sonataRepository_.GetStringFromConfiguration(edge, NETWORK_NETWORKS_EDGES_FILE);
            H5Easy::File edgesFile(edgesFileName, H5Easy::File::ReadOnly);
            cout << "Loading edges from file " << edgesFileName << '\n';

            auto edgesGroup = edgesFile.getGroup(GROUP_EDGES);
            auto populationNames = edgesGroup.listObjectNames();
            for (auto& populationName : populationNames)
            {
                auto populationGroup = edgesGroup.getGroup(populationName);
                FillEdgeGroups(populationGroup, sonataEdges);

                auto edgeGroupIdsDataset = populationGroup.getDataSet(DATASET_EDGE_GROUP_ID);
                edgeGroupIdsDataset.read(sonataEdges.EdgeGroupIds());

                auto edgeGroupIndexesDataset = populationGroup.getDataSet(DATASET_EDGE_GROUP_INDEX);
                edgeGroupIndexesDataset.read(sonataEdges.EdgeGroupIndexes());

                auto edgeGroupTypeIdsDataset = populationGroup.getDataSet(DATASET_EDGE_TYPE_ID);
                edgeGroupTypeIdsDataset.read(sonataEdges.EdgeTypeId());

                auto sourceNodeIdsDataset = populationGroup.getDataSet(DATASET_SOURCE_NODE_ID);
                FillSourceOrTargetNodes(sourceNodeIdsDataset, sonataEdges.SourceNodeIds(), sonataEdges.SourcePopulation());

                auto targetNodeIdsDataset = populationGroup.getDataSet(DATASET_TARGET_NODE_ID);
                FillSourceOrTargetNodes(targetNodeIdsDataset, sonataEdges.TargetNodeIds(), sonataEdges.TargetPopulation());

                cout 
                    << "Loaded edges for population " << populationName 
                    << " with " << sonataEdges.Groups().size() << " groups, " 
                    << sonataEdges.SourceNodeIds().size() << " nodes from source population " << sonataEdges.SourcePopulation() 
                    << " and " 
                    << sonataEdges.TargetNodeIds().size() << " nodes to target population " << sonataEdges.TargetPopulation()
                    << '\n';
            }
        }

        void FillEdgeGroups(const HighFive::Group& populationGroup, SonataEdges& sonataEdges)
        {
            auto populationObjectNames = populationGroup.listObjectNames();
            for (auto& populationObjectName : populationObjectNames)
            {
                if (regex_match(populationObjectName, regex("\\d+")))
                {
                    auto nodeType = populationGroup.getObjectType(populationObjectName);
                    if (nodeType == HighFive::ObjectType::Group)
                    {
                        auto edgeGroupValue = std::stoi(populationObjectName);
                        sonataEdges.Groups().insert(std::make_pair(edgeGroupValue, SonataEdgeGroup(edgeGroupValue)));

                        auto edgeGroup = populationGroup.getGroup(populationObjectName);
                        auto edgeGroupNsynsDataset = edgeGroup.getDataSet(DATASET_EDGE_NSYNS);

                        auto& sonataEdgeGroup = sonataEdges.Groups().at(edgeGroupValue);
                        edgeGroupNsynsDataset.read(sonataEdgeGroup.NumberOfSynapses());
                    }
                }
            }
        }

        void FillSourceOrTargetNodes(const HighFive::DataSet& nodeIdsDataset, vector<unsigned long int>& nodeIds, string& populationName)
        {
            auto populationAttribute = nodeIdsDataset.getAttribute(ATTRIBUTE_NODE_POPULATION);
            if (populationAttribute.getDataType().string() == "String64")
            {
                nodeIdsDataset.read(nodeIds);
                populationAttribute.read(populationName);
            }
        }

        void MapSonataEdgesToModelConnections(const vector<SonataEdges>& sonataEdgesCollection)
        {
            for (const auto& edges : sonataEdgesCollection)
            {
                ValidateSonataEdges(edges);
                if (!isValid_) return;
            }

            modelConnections_.Reset();
            for (const auto& edges : sonataEdgesCollection)
            {
                MapSonataEdges(edges);
            }
        }

        void ValidateSonataEdges(const SonataEdges& edges)
        {
            auto edgeCount = edges.SourceNodeIds().size();
            if (edges.TargetNodeIds().size() != edgeCount)
            {
                cout << "Source node Ids count " << edges.SourceNodeIds().size() 
                    << " (pop " << edges.SourcePopulation() 
                    << ") not equal to target node Ids count " << edges.TargetNodeIds().size() 
                    << " (pop " << edges.TargetPopulation() 
                    << ")\n";
                isValid_ = false;
                return;
            }

            if (edges.EdgeGroupIds().size() != edgeCount)
            {
                cout << "Source node Ids count " << edges.SourceNodeIds().size() 
                    << " not equal to edge group Ids count " << edges.EdgeGroupIds().size() 
                    << "\n";
                isValid_ = false;
                return;
            }

            if (edges.EdgeGroupIndexes().size() != edgeCount)
            {
                cout << "Source node Ids count " << edges.SourceNodeIds().size() 
                    << " not equal to edge group Indexes count " << edges.EdgeGroupIndexes().size() 
                    << "\n";
                isValid_ = false;
                return;
            }

            if (edges.EdgeTypeId().size() != edgeCount)
            {
                cout << "Source node Ids count " << edges.SourceNodeIds().size() 
                    << " not equal to edge type Ids count " << edges.EdgeTypeId().size() 
                    << "\n";
                isValid_ = false;
                return;
            }

            cout << "All Sonata edges elements have the same count of " << edgeCount << '\n';
            isValid_ = true;
        }

        void MapSonataEdges(const SonataEdges& edges)
        {
            auto& edgeGroupIds = edges.EdgeGroupIds();
            auto& edgeGroupIndexes = edges.EdgeGroupIndexes();
            auto& edgeGroups = edges.Groups();
            auto& edgeSourceNodeIds = edges.SourceNodeIds();
            auto& edgeTargetNodeIds = edges.TargetNodeIds();

            for (unsigned long int edgeIndex = 0; edgeIndex < edges.SourceNodeIds().size(); edgeIndex++)
            {
                const auto edgeId = edges.EdgeTypeId()[edgeIndex];
                const auto& edgeCommonInfo = edges.EdgeTypeInfo().at(edgeId);
                const double synapticWeight = edgeCommonInfo.SynapticWeight();
                const double delay = edgeCommonInfo.Delay();

                const auto edgeGroupId = edgeGroupIds[edgeIndex];
                const auto& edgeGroup = edgeGroups.at(edgeGroupId);
                const auto edgeGroupIndex = edgeGroupIndexes[edgeIndex];
                const auto numberOfSynapses = edgeGroup.NumberOfSynapses()[edgeGroupIndex];
                const auto sourceNodeId = edgeSourceNodeIds[edgeIndex];
                const auto targetNodeId = edgeTargetNodeIds[edgeIndex];

                modelConnections_.push_back(SonataFromTo(synapticWeight, delay, numberOfSynapses, 
                                                            edges.SourcePopulation(), sourceNodeId, 
                                                            edges.TargetPopulation(), targetNodeId));
            }

            cout << "Mapped " << edges.SourceNodeIds().size() << " edges for a cumulative connection count of " << modelConnections_.size() << '\n';
        }
    };
}
