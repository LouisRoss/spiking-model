#pragma once

#include <string>
#include <vector>
#include <numeric>

namespace embeddedpenguins::neuron::infrastructure::persistence::sonata
{
    using std::string;
    using std::vector;
    using std::accumulate;

    //
    // All the action types a node group can take.
    //
    enum class NodeAction
    {
        Excitatory,
        Inhibitory
    };

    //
    // The sonata node group is an abstraction describing a collection
    // of initially identical nodes, so individual nodes are not represented.
    // Each group of nodes shares attributes such as a group name and action
    // (exitatory or inhibitory).  The group id can be matched between the
    // H5 and csv files to match up attributes with the group.
    //
    class SonataNodeGroup
    {
        string nodeGroupName_;
        int id_;
        NodeAction action_;
        string modelType_;
        unsigned long int nodeCount_ {};

    public:
        const string& NodeGroupName() const { return nodeGroupName_; }
        const int Id() const { return id_; };
        const NodeAction Action() const { return action_; }
        const string& ModelType() const { return modelType_; }
        const unsigned long int NodeCount() const { return nodeCount_; }
        unsigned long int& NodeCount() { return nodeCount_; }

        SonataNodeGroup() : nodeGroupName_(""), id_(0), action_(NodeAction::Excitatory), modelType_("") { }
        SonataNodeGroup(const string& name) : nodeGroupName_(name), id_(0), action_(NodeAction::Excitatory), modelType_("") { }
        SonataNodeGroup(const string& name, int id, NodeAction action, const string& modelType) : 
            nodeGroupName_(name), 
            id_(id), 
            action_(action),
            modelType_(modelType)
            {

            }

        static NodeAction FromString(const string& action)
        { 
            if (action[0] == 'e' || action[0] == 'E') return NodeAction::Excitatory; 
            return NodeAction::Inhibitory;
        }
    };


    //
    // A Sonata population is a collection of node groups.
    // Each population also has some attributes common to all groups.
    //
    class SonataPopulation : public vector<SonataNodeGroup>
    {
        unsigned long int nodeOffset_;
        string populationName_;

    public:
        const unsigned long int NodeOffset() const { return nodeOffset_; };
        unsigned long int& NodeOffset() { return nodeOffset_; };
        const unsigned long int NodeCount() const
        {
            return accumulate(this->begin(), this->end(), 0UL, [](unsigned long int& total, const SonataNodeGroup& nodeGroup){ return total + nodeGroup.NodeCount(); });
        }
        const string& PopulationName() const { return populationName_; };
        string& PopulationName() { return populationName_; };

        SonataPopulation() : nodeOffset_(0), populationName_("") { }
        SonataPopulation(const string& name) :
            populationName_(name),
            nodeOffset_(0)
        {

        }

        const SonataNodeGroup& GetFromIndex(unsigned long int index) const
        {
            static SonataNodeGroup nullNodeGroup("null");

            unsigned long int runningTotal {};
            for (auto& node : *this)
            {
                runningTotal += node.NodeCount();
                if (index < runningTotal) return node;
            }

            return nullNodeGroup;
        }

        SonataNodeGroup& FindNodeGroup(unsigned long int nodeTypeId)
        {
            static SonataNodeGroup nullGroup("null");

            for (auto& nodeGroup : *this)
            {
                if (nodeGroup.Id() == nodeTypeId) return nodeGroup;
            }

            return nullGroup;
        }
    };

    //
    // The outer Sonata nodes entity, a collection of populations.
    //
    class SonataPopulationCollection : public vector<SonataPopulation>
    {
    public:
        void SetPopulationNodeOffsets()
        {
            unsigned long int offset {};
            for (auto& population : *this)
            {
                population.NodeOffset() = offset;
                offset += population.NodeCount();
            }
        }

        const unsigned long int NodeCount() const
        {
            return accumulate(this->begin(), this->end(), 0UL, [](unsigned long int& total, const SonataPopulation& population){ return total + population.NodeCount(); });
        }

        const SonataNodeGroup& GetFromIndex(unsigned long int index) const
        {
            static SonataNodeGroup nullNodeGroup("null");

            unsigned long int runningTotal {};
            for (auto& population : *this)
            {
                runningTotal += population.NodeCount();
                if (index = runningTotal) return population.GetFromIndex(index);
            }

            return nullNodeGroup;
        }

        SonataPopulation& FindPopulation(const string& populationName)
        {
            static SonataPopulation nullPopulation("null");

            for (auto& population : *this)
            {
                if (population.PopulationName() == populationName) return population;
            }

            return nullPopulation;
        }
    };
}
