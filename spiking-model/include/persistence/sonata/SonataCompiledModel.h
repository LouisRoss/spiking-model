#pragma once

#include <string>
#include <vector>

namespace embeddedpenguins::neuron::infrastructure::persistence::sonata
{
    using std::string;
    using std::vector;

    //
    // After compilation, the sonata edges descriptions get
    // converted into a collection of these SonataFromTo objects.
    // Each SonataFromTo object can be converted to a model synapse.
    //
    class SonataFromTo
    {
        double synapticWeight_;
        double delay_;
        unsigned int numberOfSynapses_ {};
        string sourcePopulation_ {};
        unsigned long int sourceNodeId_ {};
        string targetPopulation_ {};
        unsigned long int targetNodeId_ {};

    public:
        const double SynapticWeight() const { return synapticWeight_; }
        const double Delay() const { return delay_; }
        const unsigned int NumberOfSynapses() const { return numberOfSynapses_; }
        const string& SourcePopulation() const { return sourcePopulation_; }
        const unsigned long int SourceNodeId() const { return sourceNodeId_; }
        const string& TargetPopulation() const { return targetPopulation_; }
        const unsigned long int TargetNodeId() const { return targetNodeId_; }

    public:
        SonataFromTo(double synapticWeight, double delay, unsigned int numberOfSynapses, 
                        const string& sourcePopulation, unsigned long int sourceNodeId, 
                        const string& targetPopulation, unsigned long int targetNodeId) :
            synapticWeight_(synapticWeight),
            delay_(delay),
            numberOfSynapses_(numberOfSynapses),
            sourcePopulation_(sourcePopulation),
            sourceNodeId_(sourceNodeId),
            targetPopulation_(targetPopulation),
            targetNodeId_(targetNodeId)
        {

        }
    };

    //
    // This collection of SonataFromTo objects represents the 
    // final compilation of information from all sonata files 
    // to describe all connections in the model.
    //
    class SonataConnections : public vector<SonataFromTo>
    {
    public:
        void Reset()
        {
            this->clear();
        }
    };
}
