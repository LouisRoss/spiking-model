#pragma once

#include <iostream>
#include <fstream>
#include <map>

#include "nlohmann/json.hpp"

#include "ModelEngineCommon.h"

#include "SensorInput/ISensorInput.h"

namespace embeddedpenguins::neuron::infrastructure::sensorinput
{
    using std::cout;
    using std::ifstream;
    using std::multimap;
    using std::make_pair;

    using nlohmann::json;

    using embeddedpenguins::modelengine::ConfigurationUtilities;

    class SensorInputFile : public ISensorInput
    {
        const ConfigurationUtilities& configuration_;
        nlohmann::ordered_json inputStream_ {};
        multimap<int, unsigned long long int> signalToInject_ {};
        vector<unsigned long long> signalToReturn_ {};

    public:
        SensorInputFile(const ConfigurationUtilities& configuration) :
            configuration_(configuration)
        {
        }

        // ISensorInput implementaton
        virtual bool Connect(const string& connectionString) override
        {
            auto sensorFile = connectionString;
            auto& postProcessingSection = configuration_.Configuration()["PostProcessing"];
            if (!postProcessingSection.is_null() && postProcessingSection.contains("SensorInputFile"))
            {
                const auto& sensorInputFile = postProcessingSection["SensorInputFile"];
                if (sensorInputFile.is_string())
                    sensorFile = sensorInputFile.get<string>();
            }

            if (sensorFile.empty())
            {
                cout << "Connect found no sensor file named or configured, cannot connect\n";
                return false;
            }

            sensorFile = configuration_.ExtractRecordDirectory() + sensorFile;
            cout << "Connect using sensor file " << sensorFile << "\n";

            ifstream sensorStream(sensorFile);
            if (!sensorStream) return false;

            sensorStream >> inputStream_;
            cout << inputStream_ << "\n";
            return true;
        }

        virtual bool Disconnect() override
        {
            return true;
        }

        virtual vector<unsigned long long>& StreamInput(unsigned long long int tickNow) override
        {
            signalToReturn_.clear();

            auto done = inputStream_.empty();
            while (!done)
            {
                auto nextSignal = inputStream_.begin();
                auto nextSignalTick = std::stoi(nextSignal.key());
                if (nextSignalTick < 0 || nextSignalTick <= tickNow)
                {
                    auto& indexList = nextSignal.value();
                    if (indexList.is_array())
                    {
                        const auto& indexes = indexList.get<vector<int>>();
                        signalToReturn_.insert(signalToReturn_.end(), indexes.begin(), indexes.end());
                    }

                    inputStream_.erase(nextSignal);
                    done = inputStream_.empty();
                }
                else
                {
                    done = true;
                }
            };

/*
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
*/
            return signalToReturn_;
        }

    private:
    };
}
