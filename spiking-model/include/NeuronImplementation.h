#pragma once

#include <vector>
#include <algorithm>
#include <iostream>
#include <math.h>
#include <chrono>

#include "nlohmann/json.hpp"

#include "WorkerThread.h"
#include "WorkItem.h"
#include "ProcessCallback.h"
#include "Recorder.h"
#include "NeuronOperation.h"
#include "NeuronNode.h"
#include "CpuModelCarrier.h"
#include "NeuronRecord.h"
#include "Log.h"

namespace embeddedpenguins::neuron::infrastructure
{
    using std::vector;
    using std::pair;
    using std::for_each;
    using std::cout;
    using std::floor;
    using std::chrono::milliseconds;
    using time_point = std::chrono::high_resolution_clock::time_point;

    using nlohmann::json;

    using ::embeddedpenguins::modelengine::threads::WorkerThread;
    using ::embeddedpenguins::modelengine::threads::ProcessCallback;
    using ::embeddedpenguins::modelengine::Log;
    using ::embeddedpenguins::modelengine::Recorder;
    using ::embeddedpenguins::modelengine::WorkItem;

    class NeuronImplementation : public WorkerThread<NeuronOperation, NeuronImplementation, NeuronRecord>
    {
        int workerId_;
        CpuModelCarrier carrier_;
        const json& configuration_;

    public:
        NeuronImplementation() = delete;

        // Required constructor.
        // Allow the template library to pass in the model
        // for each worker thread that is created.
        NeuronImplementation(int workerId, CpuModelCarrier carrier, const json& configuration) :
            workerId_(workerId),
            carrier_(carrier),
            configuration_(configuration)
        {
            
        }

        void Initialize(Log& log, Recorder<NeuronRecord>& record, 
            unsigned long long int tickNow, 
            ProcessCallback<NeuronOperation, NeuronRecord>& callback)
        {
        }

        void Process(Log& log, Recorder<NeuronRecord>& record, 
            unsigned long long int tickNow, 
            typename vector<WorkItem<NeuronOperation>>::iterator begin, 
            typename vector<WorkItem<NeuronOperation>>::iterator end, 
            ProcessCallback<NeuronOperation, NeuronRecord>& callback)
        {
            if (end - begin == 0) return;

            for (auto& work = begin; work != end; work++)
            {
                ProcessWorkItem(log, record, tickNow, work->Operator, callback);
            }
        }

        void Finalize(Log& log, Recorder<NeuronRecord>& record, unsigned long long int tickNow)
        {
        }

    private:
        void ProcessWorkItem(Log& log, Recorder<NeuronRecord>& record, 
            unsigned long long int ticksSinceEpoch, 
            const NeuronOperation& work, 
            ProcessCallback<NeuronOperation, NeuronRecord>& callback)
        {
            switch (work.Op)
            {
            // The neuron at work.Index received a synaptic contribution with strenth work.Data.
            case Operation::Spike:
                ProcessSpike(log, record, ticksSinceEpoch, work.Index, work.Data, callback);
                break;

            // The neuron at work.Index is in decay phase.
            case Operation::Decay:
                ProcessDecay(log, record, work.Index, callback);
                break;

            // The neuron at work.Index is in refractory phase.
            case Operation::Refractory:
                ProcessRefractory(log, record, work.Index, callback);
                break;

            default:
                break;
            }
        }

        //
        // The neuron at neuronIndex/synapseIndex received a synaptic contribution
        // with the current synapse strength.  Bump the activation accordingly, 
        // spike if the threshold is passed.
        //
        void ProcessSpike(Log& log, Recorder<NeuronRecord>& record, 
            unsigned long long int tickNow, 
            unsigned long long int neuronIndex, 
            int synapseIndex, 
            ProcessCallback<NeuronOperation, NeuronRecord>& callback)
        {
            auto& neuronNode = carrier_.Model[neuronIndex];

            if (neuronNode.InRefractoryDelay)
            {
#ifndef NOLOG
                log.Logger() << "Neuron " << neuronIndex << ", " << NeuronSynapse::ToString(neuronNode.Synapses[synapseIndex].Type) << " synapse " << synapseIndex  << " in refractory delay, skipping\n";
                log.Logit();
#endif
                return;
            }

            switch (neuronNode.Synapses[synapseIndex].Type)
            {
                case SynapseType::Excitatory:
                    neuronNode.Activation += neuronNode.Synapses[synapseIndex].Strength;
                    break;
                case SynapseType::Inhibitory:
                    neuronNode.Activation -= neuronNode.Synapses[synapseIndex].Strength;
                    if (neuronNode.Activation < -50) neuronNode.Activation = -50;
                    break;
                case SynapseType::Attention:
                default:
                    break;
            }
#ifndef NOLOG
            log.Logger() << "Neuron " << neuronIndex << ", " << NeuronSynapse::ToString(neuronNode.Synapses[synapseIndex].Type) << " synapse " << synapseIndex  << " got signal with strength " << neuronNode.Synapses[synapseIndex].Strength << ", resulting in activation " << neuronNode.Activation << '\n';
            log.Logit();
#endif
            record.Record(NeuronRecord(NeuronRecordType::InputSignal, neuronIndex, synapseIndex, neuronNode));

            if (neuronNode.Activation > ActivationThreshold)
            {
                record.Record(NeuronRecord(NeuronRecordType::Spike, neuronIndex, 0, neuronNode));
                std::stringstream os;
                os << "  ";
                SignalAllPostsynapticConnections(log, record, os, tickNow, neuronNode, callback);
#ifndef NOLOG
                log.Logger() << os.str() << '\n';
                log.Logit();
#endif
                AdjustSynapticWeightsUpward(log, record, tickNow, neuronIndex, neuronNode);

                callback(NeuronOperation(neuronIndex, Operation::Refractory), RefractoryTime);
                neuronNode.TickLastSpike = tickNow;
                neuronNode.Activation = ActivationThreshold;
                neuronNode.InRefractoryDelay = true;
            }
            else
            {
                AdjustSynapticWeightsDownward(log, record, tickNow, neuronIndex, neuronNode, synapseIndex);
                if (!neuronNode.InDecay)
                {
                    neuronNode.InDecay = true;
                    callback(NeuronOperation(neuronIndex, Operation::Decay), DecayProcessRate);
                }
            }

            neuronNode.Synapses[synapseIndex].TickLastSignal = tickNow;
        }

        void SignalAllPostsynapticConnections(Log& log, Recorder<NeuronRecord>& record, 
            std::stringstream& os, 
            unsigned long long int tickNow, 
            NeuronNode& neuronNode, 
            ProcessCallback<NeuronOperation, NeuronRecord>& callback)
        {
            for (auto& connection : neuronNode.PostsynapticConnections)
            {
                if (connection.IsUsed) 
                {
                    //log.Logger() << "Neuron " << neuronIndex << " sending spike to neuron " << connection.PostsynapticNeuron << ", synapse " << connection.Synapse << '\n';
                    //log.Logit();
                    os << connection.PostsynapticNeuron << ":" << connection.Synapse << ' ';
                    callback(NeuronOperation(connection.PostsynapticNeuron, Operation::Spike, connection.Synapse), SignalDelayTime);
                }
            }
        }

        void AdjustSynapticWeightsUpward(Log& log, Recorder<NeuronRecord>& record, 
            unsigned long long int tickNow, 
            unsigned long long int neuronIndex, 
            NeuronNode& neuronNode)
        {
            auto synapseIndex = 0;
            for (auto synapseIndex = 0; synapseIndex < PresynapticConnectionsPerNode; synapseIndex++)
            {
                auto& synapse = neuronNode.Synapses[synapseIndex];
                if (synapse.IsUsed && synapse.Type != SynapseType::Inhibitory)
                {
                    // Both timers use the same monotonic increment with rollover.  The difference
                    // will always be valid, even if one has rolled over.
                    auto ticksSinceSignal = tickNow - (unsigned long long int)synapse.TickLastSignal;
                    if (ticksSinceSignal < PostsynapticPlasticityPeriod)
                    {
                        auto newStrength = synapse.Strength * PostsynapticIncreaseFunction[ticksSinceSignal];
                        if (newStrength > MaxSynapseStrength) newStrength = MaxSynapseStrength;
                        if (newStrength < MinSynapseStrength) newStrength = MinSynapseStrength;
                        synapse.Strength = newStrength;
#ifndef NOLOG
                        log.Logger() 
                            << " Synapse index " << synapseIndex 
                            << ", bumped by " << PostsynapticIncreaseFunction[ticksSinceSignal] 
                            << ", resulting in strength " << synapse.Strength 
                            << '\n';
                        log.Logit();
#endif
                        //record.Record(NeuronRecord(neuronIndex, synapseIndex, neuronNode));
                    }
                }
            }
        }

        void AdjustSynapticWeightsDownward(Log& log, Recorder<NeuronRecord>& record, 
            unsigned long long int tickNow, 
            unsigned long long int neuronIndex, 
            NeuronNode& neuronNode, 
            int synapseIndex)
        {
            if (neuronNode.Synapses[synapseIndex].Type == SynapseType::Inhibitory) return;

            // Both timers use the same monotonic increment with rollover.  The difference
            // will always be valid, even if one has rolled over.
            auto ticksSinceSpike = tickNow - neuronNode.TickLastSpike;
            if (ticksSinceSpike < PostsynapticPlasticityPeriod)
            {
                neuronNode.Synapses[synapseIndex].Strength *= PostsynapticDecreaseFunction[ticksSinceSpike];

#ifndef NOLOG
                log.Logger() 
                    << " Synapse index " << synapseIndex 
                    << ", tick = " << tickNow << " - " << neuronNode.TickLastSpike << " = " << ticksSinceSpike
                    << ", reduced by " << PostsynapticDecreaseFunction[ticksSinceSpike] 
                    << ", resulting in strength " << neuronNode.Synapses[synapseIndex].Strength 
                    << '\n';
                log.Logit();
#endif
                //record.Record(NeuronRecord(neuronIndex, synapseIndex, neuronNode));
            }
        }

        //
        // The neuron at neuronIndex has recently had a nonzero activation.
        // Decay the current activation until zero.
        //
        void ProcessDecay(Log& log, Recorder<NeuronRecord>& record, 
            unsigned long long int neuronIndex, 
            ProcessCallback<NeuronOperation, NeuronRecord>& callback)
        {
            auto& neuronNode = carrier_.Model[neuronIndex];

            if (neuronNode.Activation == 0 || neuronNode.InRefractoryDelay)
            {
#ifndef NOLOG
                log.Logger() << "Neuron " << neuronIndex << " decayed to " << neuronNode.Activation << ", with refractory delay " << (neuronNode.InRefractoryDelay? "true" : "false") << ", skipping\n";
                log.Logit();
#endif
                return;
            }

            double activation = neuronNode.Activation;
            activation *= DecayRate;
            if (activation >= 0.0)
                neuronNode.Activation = (int)floor(activation);
            else
                neuronNode.Activation = (int)ceil(activation);
            
#ifndef NOLOG
            log.Logger() << "Neuron " << neuronIndex << " decayed to " << neuronNode.Activation << "\n";
            log.Logit();
#endif
            record.Record(NeuronRecord(NeuronRecordType::Decay,  neuronIndex, -1, neuronNode));

            if (std::abs(neuronNode.Activation) > 0)
            {
                callback(NeuronOperation(neuronIndex, Operation::Decay), DecayProcessRate);
            }
            else
            {
                neuronNode.InDecay = false;
            }
        }

        //
        // The neuron at neuronIndex has recently spiked, and the refractory timer was set.
        // The refractory period is now over, return to normal operation.
        //
        void ProcessRefractory(Log& log, Recorder<NeuronRecord>& record, 
            unsigned long long int neuronIndex, 
            ProcessCallback<NeuronOperation, NeuronRecord>& callback)
        {
            auto& neuronNode = carrier_.Model[neuronIndex];

            if (!neuronNode.InRefractoryDelay)
            {
#ifndef NOLOG
                log.Logger() << "Neuron " << neuronIndex << " not in refractory delay, skipping\n";
                log.Logit();
#endif
                return;
            }

#ifndef NOLOG
            log.Logger() << "Neuron " << neuronIndex << " refractory delay complete\n";
            log.Logit();
#endif
            neuronNode.InRefractoryDelay = false;
            neuronNode.InDecay = false;
            neuronNode.Activation = 0;
            record.Record(NeuronRecord(NeuronRecordType::Refractory, neuronIndex, -1, neuronNode));
        }
    };
}
