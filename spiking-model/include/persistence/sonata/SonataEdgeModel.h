#pragma once

#include <string>
#include <vector>
#include <map>

namespace embeddedpenguins::neuron::infrastructure::persistence::sonata
{
    using std::string;
    using std::vector;
    using std::map;

    //
    // Sonata saves information common to many edges in a CSV file
    // keyed by edge type.  For each edge type, extract the needed
    // common information.
    //
    class SonataEdgeTypeCommon
    {
        int id_;
        double synapticWeight_;
        double delay_;

    public:
        const int& Id() const { return id_; }
        const double& SynapticWeight() const { return synapticWeight_; }
        const double& Delay() const { return delay_; }

    public:
        SonataEdgeTypeCommon(int id, double weight, double delay) :
            id_(id),
            synapticWeight_(weight),
            delay_(delay)
        {

        }
    };

    //
    // Edges are grouped by a unique group number, with information
    // common to the group.  Read the group information from the H5
    // file and store here.
    // 
    class SonataEdgeGroup
    {
        int groupNumber_ {};
        vector<unsigned int> numberOfSynapses_ {};

    public:
        const vector<unsigned int>& NumberOfSynapses() const { return numberOfSynapses_; }
        vector<unsigned int>& NumberOfSynapses() { return numberOfSynapses_; }

    public:
        SonataEdgeGroup(int groupNumber) :
            groupNumber_(groupNumber)
        {
            
        }
    };

    //
    // Collect all information from both the CSV and H5 files for all edges.
    //
    class SonataEdges
    {
        map<int, SonataEdgeTypeCommon> edgeTypeInfo_ {};
        map<unsigned int, SonataEdgeGroup> groups_ {};
        vector<unsigned int> edgeGroupIds_ {};
        vector<unsigned long int> edgeGroupIndexes_ {};
        vector<unsigned long int> edgeTypeId_ {};
        string sourcePopulation_ {};
        vector<unsigned long int> sourceNodeIds_ {};
        string targetPopulation_ {};
        vector<unsigned long int> targetNodeIds_ {};

    public:
        const map<int, SonataEdgeTypeCommon>& EdgeTypeInfo() const { return edgeTypeInfo_; }
        map<int, SonataEdgeTypeCommon>& EdgeTypeInfo() { return edgeTypeInfo_; }
        const map<unsigned int, SonataEdgeGroup>& Groups() const { return groups_; }
        map<unsigned int, SonataEdgeGroup>& Groups() { return groups_; }
        const vector<unsigned int>& EdgeGroupIds() const { return edgeGroupIds_; }
        vector<unsigned int>& EdgeGroupIds() { return edgeGroupIds_; }
        const vector<unsigned long int>& EdgeGroupIndexes() const { return edgeGroupIndexes_; }
        vector<unsigned long int>& EdgeGroupIndexes() { return edgeGroupIndexes_; }
        const vector<unsigned long int>& EdgeTypeId() const { return edgeTypeId_; }
        vector<unsigned long int>& EdgeTypeId() { return edgeTypeId_; }
        const string& SourcePopulation() const { return sourcePopulation_; }
        string& SourcePopulation() { return sourcePopulation_; }
        const vector<unsigned long int>& SourceNodeIds() const { return sourceNodeIds_; }
        vector<unsigned long int>& SourceNodeIds() { return sourceNodeIds_; }
        const string& TargetPopulation() const { return targetPopulation_; }
        string& TargetPopulation() { return targetPopulation_; }
        const vector<unsigned long int>& TargetNodeIds() const { return targetNodeIds_; }
        vector<unsigned long int>& TargetNodeIds() { return targetNodeIds_; }
    };
}
