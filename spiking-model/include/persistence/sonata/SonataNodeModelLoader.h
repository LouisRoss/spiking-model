#pragma once

#include <vector>
#include <map>

#include "nlohmann/json.hpp"
#include "csv-parser/csv.hpp"
#include "highfive/H5Easy.hpp"

#include "SonataModelRepository.h"
#include "SonataNodeModel.h"

namespace embeddedpenguins::neuron::infrastructure::persistence::sonata
{
    using nlohmann::json;
    using std::vector;
    using std::map;

    class SonataNodeModelLoader
    {
        bool isValid_ { false };

        SonataPopulationCollection populations_ {};
        const SonataModelRepository& sonataRepository_;

    public:
        const bool IsValid() const { return isValid_; }
        const SonataPopulationCollection& Populations() const { return populations_; }
        SonataPopulationCollection& Populations() { return populations_; }

    public:
        SonataNodeModelLoader(const SonataModelRepository& repository) :
            sonataRepository_(repository)
        {
        }

        void BuildPopulations(const json& nodes)
        {
            for (const auto& node : nodes)
            {
                const auto nodesFileName = sonataRepository_.GetStringFromConfiguration(node, NETWORK_NETWORKS_NODES_FILE);
                SonataPopulation population {};
                FillPopulationGroups(node, population);
                CountNodeGroups(nodesFileName, population);

                populations_.push_back(population);
                cout << "Created population " << population.PopulationName() << " with " << population.size() << " node groups\n";
            }
        }

    private:
        void FillPopulationGroups(const json& node, SonataPopulation& population)
        {
            auto nodetypesFileName = sonataRepository_.GetStringFromConfiguration(node, NETWORK_NETWORKS_NODE_TYPES_FILE);
            csv::CSVFormat format;
            format.delimiter(' ');
            csv::CSVReader reader(nodetypesFileName, format);
            for (auto& row : reader)
            {
                auto columnNames = row.get_col_names();

                int typeId {};
                string typeName {};
                string typeEI {"e"};
                string modelType {""};

                if (find_if(begin(columnNames), end(columnNames), [](string cn){ return cn == "node_type_id"; }) != columnNames.end())
                    typeId = row["node_type_id"].get<int>();
                if (find_if(begin(columnNames), end(columnNames), [](string cn){ return cn == "pop_name"; }) != columnNames.end())
                    typeName = row["pop_name"].get<string>();
                if (find_if(begin(columnNames), end(columnNames), [](string cn){ return cn == "ei"; }) != columnNames.end())
                    typeEI = row["ei"].get<string>();
                if (find_if(begin(columnNames), end(columnNames), [](string cn){ return cn == "model_type"; }) != columnNames.end())
                    modelType = row["model_type"].get<string>();

               population.push_back(SonataNodeGroup(typeName, typeId, SonataNodeGroup::FromString(typeEI), modelType));
            }
        }

        void CountNodeGroups(const string& nodesFileName, SonataPopulation& population)
        {
            H5Easy::File nodesFile(nodesFileName, H5Easy::File::ReadOnly);
            auto nodesGroup = nodesFile.getGroup(GROUP_NODES);
            auto populationNames = nodesGroup.listObjectNames();
            for (auto& populationName : populationNames)
            {
                population.PopulationName() = populationName;

                auto populationGroup = nodesGroup.getGroup(populationName);
                auto nodeTypeIdDataset = populationGroup.getDataSet(DATASET_NODE_TYPE_ID);
                vector<unsigned int> nodeTypeIds;
                nodeTypeIdDataset.read(nodeTypeIds);

                map<unsigned int, unsigned long int> edgeTypeIdsAndCounts;
                CollectSameIds(nodeTypeIds, edgeTypeIdsAndCounts);
                for (auto [nodeTypeId, nodeTypeCount] : edgeTypeIdsAndCounts)
                {
                    auto& nodeGroup = population.FindNodeGroup(nodeTypeId);
                    if (nodeGroup.NodeGroupName() != "null")
                    {
                        nodeGroup.NodeCount() = nodeTypeCount;
                        cout << "Found node group id " << nodeGroup.Id() << " (" << nodeGroup.NodeGroupName() << ") with " << nodeGroup.NodeCount() << " nodes\n";
                    }
                    else
                    {
                        cout << "Unable to find node group id " << nodeTypeId << " in population " << population.PopulationName() << ", cannot continue\n";
                        isValid_ = false;
                    }
                }
            }
        }

        void CollectSameIds(const vector<unsigned int>& ids, map<unsigned int, unsigned long int>& sameIds) const
        {
            for (auto id : ids)
                if (sameIds.find(id) == sameIds.end())
                    sameIds[id] = 1;
                else
                    sameIds[id]++;
        }
    };
}
