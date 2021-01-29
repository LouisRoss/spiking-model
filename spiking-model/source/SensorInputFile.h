#pragma once
#include <map>

#include "nlohmann/json.hpp"

#include "SensorInput/ISensorInput.h"

namespace embeddedpenguins::neuron::infrastructure::sensorinput
{
    using std::multimap;
    using std::make_pair;
    using nlohmann::json;

    class SensorInputFile : public ISensorInput
    {
        const json& configuration_;
        multimap<int, unsigned long long int> signalToInject_ {};
        vector<unsigned long long> signalToReturn_ {};

    public:
        SensorInputFile(const json& configuration) :
            configuration_(configuration)
        {
            // Test only, remove.
            for (int i = 1; i < 6; i++)
            {
                signalToInject_.insert(make_pair((i*3)+0, 1));
                signalToInject_.insert(make_pair((i*3)-1, 2));
                signalToInject_.insert(make_pair((i*3)-2, 3));
                signalToInject_.insert(make_pair((i*3)-3, 4));
                signalToInject_.insert(make_pair((i*3)-4, 5));
            }
        }

        // ISensorInput implementaton
        virtual bool Connect(const string& connectionString) override
        {
            return true;
        }

        virtual bool Disconnect() override
        {
            return true;
        }

        virtual vector<unsigned long long>& StreamInput(unsigned long long int tickNow) override
        {
            signalToReturn_.clear();

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

            return signalToReturn_;
        }

    private:
    };
}
