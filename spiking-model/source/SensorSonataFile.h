#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <memory>

#include "nlohmann/json.hpp"

#include "ModelEngineCommon.h"
#include "sdk/IModelPersister.h"
#include "persistence/sonata/SonataModelRepository.h"
#include "persistence/sonata/SonataModelPersister.h"
#include "persistence/sonata/SonataInputSpikeLoader.h"

#include "SensorInput/ISensorInput.h"

//#define TESTING
namespace embeddedpenguins::neuron::infrastructure::sensorinput
{
    using std::cout;
    using std::ifstream;
    using std::multimap;
    using std::make_pair;
    using std::unique_ptr;
    using std::make_unique;

    using nlohmann::json;

    using embeddedpenguins::modelengine::ConfigurationUtilities;
    using embeddedpenguins::neuron::infrastructure::persistence::IModelPersister;
    using embeddedpenguins::neuron::infrastructure::persistence::sonata::SonataModelRepository;
    using embeddedpenguins::neuron::infrastructure::persistence::sonata::SonataModelPersister;
    using embeddedpenguins::neuron::infrastructure::persistence::sonata::SonataInputSpikeLoader;

    class SensorSonataFile : public ISensorInput
    {
        const ConfigurationUtilities& configuration_;
        nlohmann::ordered_json inputStream_ {};

        unique_ptr<SonataModelRepository> sonataRepository_ {nullptr};
        unique_ptr<IModelPersister<CpuModelCarrier>> persister_ {nullptr};

        multimap<int, unsigned long long int> signalToInject_ {};
        vector<unsigned long long> signalToReturn_ {};

    public:
        SensorSonataFile(const ConfigurationUtilities& configuration) :
            configuration_(configuration)
        {
        }

        // ISensorInput implementaton
        virtual bool Connect(const string& connectionString) override
        {
            cout << "SensorSonataFile Connect(" << connectionString << ")\n";

            auto& modelSection = this->configuration_.Configuration()["Model"];
            if (modelSection.is_null())
            {
                cout << "SensorSonataFile did not find 'Model' section\n";
                return false;
            }

            auto& sonataConfigurationElement = modelSection["SonataConfiguration"];
            if (sonataConfigurationElement.is_null())
            {
                cout << "SensorSonataFile did not find 'SonataConfiguration' element in 'Model' section\n";
                return false;
            }

            if (!sonataConfigurationElement.is_string())
            {
                cout << "ModelSonataInitializer unable to find sonata configuration file path\n";
                return false;
            }
            auto sonataConfiguration = sonataConfigurationElement.get<string>();
            cout << "SensorSonataFile connecting with 'SonataConfiguration' file: " << sonataConfiguration << "\n";

            sonataRepository_ = make_unique<SonataModelRepository>(sonataConfiguration);
            cout << "SensorSonataFile created sonata repository\n";
            persister_ = make_unique<SonataModelPersister>(sonataConfiguration, *sonataRepository_);
            cout << "SensorSonataFile created sonata persister\n";

            persister_->LoadConfiguration();
            //persister_->ReadModel(this->helper_.Carrier(), configuration_);

            SonataInputSpikeLoader spikeLoader(*sonataRepository_);
            cout << "SensorSonataFile created sonata spike loader\n";
            spikeLoader.LoadSpikes(500);  // Is this number available in the LGN part of the configuration?
            cout << "SensorSonataFile loaded spikes from sonata spike loader\n";

            auto& spikeTimesAndNodes = spikeLoader.Spikes();
            cout << "SensorSonataFile spike loader loaded " << spikeTimesAndNodes.size() << " spike/time pairs\n";
            for (auto [spikeTime, spikeNode] : spikeTimesAndNodes)
            {
                signalToInject_.insert(make_pair(spikeTime + 1000, spikeNode));
            }

            return true;
        }

        virtual bool Disconnect() override
        {
            return true;
        }

        virtual vector<unsigned long long>& StreamInput(unsigned long long int tickNow) override
        {
#ifndef TESTING
            auto done = signalToInject_.empty();
            while (!done)
            {
                auto nextSignal = signalToInject_.begin();
                if (nextSignal->first < 0 || nextSignal->first <= tickNow)
                {
                    signalToReturn_.push_back(nextSignal->second);
                    signalToInject_.extract(nextSignal);
                    done = signalToInject_.empty();
                }
                else
                {
                    done = true;
                }
            };

#else
            for (auto i = 0; i < 3000; i += 250)
            {
                    callback(NeuronOperation(47, Operation::Spike, 0), i);
                    callback(NeuronOperation(285, Operation::Spike, 0), i);
                    callback(NeuronOperation(312, Operation::Spike, 0), i);
                    callback(NeuronOperation(471, Operation::Spike, 0), i);
            }
#endif
            return signalToReturn_;
        }

    private:
    };
}
