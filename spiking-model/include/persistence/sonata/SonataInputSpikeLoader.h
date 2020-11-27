#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <regex>

#include "nlohmann/json.hpp"
#include "highfive/H5Easy.hpp"

#include "SonataModelRepository.h"

namespace embeddedpenguins::neuron::infrastructure::persistence::sonata
{
    using std::cout;
    using std::ofstream;
    using std::vector;
    using std::multimap;
    using std::pair;
    using std::regex;
    using std::regex_match;
    using nlohmann::json;

    class SonataInputSpikeLoader
    {
        const SonataModelRepository& sonataRepository_;
        bool isValid_ { false };
        multimap<double, unsigned long int> spikes_ {};

    public:
        const multimap<double, unsigned long int>& Spikes() const { return spikes_; }
        const bool IsValid() const { return isValid_; }

    public:
        SonataInputSpikeLoader(const SonataModelRepository& repository) :
            sonataRepository_(repository)
        {
            
        }

        bool LoadSpikes(int nodeCount)
        {
            auto& simulationConfiguration = sonataRepository_.GetConfiguration(SIMULATION_CONFIGURATION_NAME);
            auto inputs = simulationConfiguration[NETWORK_INPUTS];
            auto success = true;
            for (auto& inputClass : inputs)
            {
                auto input_type = inputClass["input_type"].get<string>();
                if (input_type != "spikes") continue;

                auto module = inputClass["module"].get<string>();
                if (module == "nwb")
                {
                    auto input_file = sonataRepository_.GetStringFromConfiguration(inputClass, "input_file");
                    auto trial = inputClass["trial"].get<string>();

                    success &= LoadNwbSpikes(input_file, trial, nodeCount);
                    cout << "Loaded spikes from NWB file" << input_file << " for " << nodeCount << " nodes with " << (success ? "success" : "failure") << '\n';
                }
                else if (module == "csv")
                {
                    // TODO
                    cout << "Input spikes from CSV not supported\n";
                }
            }

            return success;            
        }

        bool PersistSpikes(const string& fileName)
        {
            ofstream spikeFile;
            spikeFile.open (fileName, std::ios::out);
            spikeFile << "neuron tick\n";
            for (auto [spikeTime, spikeNode] : spikes_)
            {
                spikeFile << spikeNode << ' ' << (int)spikeTime << '\n';
            }
            spikeFile.close();

            return true;
        }

    private:
        bool LoadNwbSpikes(const string& spikesFileName, const string& trial, int nodeCount)
        {
            H5Easy::File spikesFile(spikesFileName, H5Easy::File::ReadOnly);
            cout << "Loading NWB spikes from file " << spikesFileName << '\n';

            auto processingTrialGroup = spikesFile.getGroup(string(GROUP_PROCESSING) + "/" + trial + "/" + GROUP_SPIKE_TRAIN);
            auto nodeNumberNames = processingTrialGroup.listObjectNames();
            for (auto& nodeName : nodeNumberNames)
            {
                if (regex_match(nodeName, regex("\\d+")))
                {
                    auto nodeNumber = std::stoi(nodeName);
                    if (nodeNumber >= nodeCount) continue;

                    auto nodeGroup = processingTrialGroup.getGroup(nodeName);
                    auto spikeTimesDataset = nodeGroup.getDataSet(DATASET_DATA);

                    vector<double> spikeTimesForNode {};
                    spikeTimesDataset.read(spikeTimesForNode);

                    for (auto spikeTime : spikeTimesForNode)
                    {
                        spikes_.insert(pair<double, unsigned long int>(spikeTime, nodeNumber));
                    }
                }
            }

            cout << "Inserted a total of " << spikes_.size() << " spikes\n";
            return true;
        }
    };
}
