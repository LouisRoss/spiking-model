#include <memory>
#include <iostream>
#include <string>
#include <chrono>
#include <stdio.h>

#include "nlohmann/json.hpp"
#include "highfive/H5Easy.hpp"

#include "ConfigurationRepository.h"
#include "ModelEngine.h"
#include "sdk/ModelRunner.h"
#include "sdk/KeyListener.h"

#include "NeuronOperation.h"
#include "NeuronImplementation.h"
#include "NeuronNode.h"
#include "NeuronRecord.h"
#include "CpuModelCarrier.h"
#include "NeuronModelHelper.h"
#include "persistence/sonata/SonataModelRepository.h"
#include "persistence/sonata/SonataModelPersister.h"
#include "persistence/sonata/SonataInputSpikeLoader.h"

using namespace std;
using namespace std::chrono;
using namespace nlohmann;

using embeddedpenguins::core::neuron::model::KeyListener;
using embeddedpenguins::core::neuron::model::ConfigurationRepository;

using embeddedpenguins::modelengine::ModelEngine;
using embeddedpenguins::modelengine::sdk::ModelRunner;

using embeddedpenguins::neuron::infrastructure::Operation;
using embeddedpenguins::neuron::infrastructure::NeuronOperation;
using embeddedpenguins::neuron::infrastructure::NeuronImplementation;
using embeddedpenguins::neuron::infrastructure::NeuronNode;
using embeddedpenguins::neuron::infrastructure::NeuronRecord;
using embeddedpenguins::neuron::infrastructure::CpuModelCarrier;
using embeddedpenguins::neuron::infrastructure::NeuronModelHelper;

using embeddedpenguins::neuron::infrastructure::persistence::sonata::SonataModelRepository;
using embeddedpenguins::neuron::infrastructure::persistence::sonata::SonataModelPersister;
using embeddedpenguins::neuron::infrastructure::persistence::sonata::SonataInputSpikeLoader;
//using std::vector;
//using std::chrono::nanoseconds;
//using std::chrono::system_clock;
//using std::chrono::high_resolution_clock;
//using std::chrono::duration_cast;
//using std::chrono::milliseconds;
//using std::chrono::ceil;

//////////////////////////////////////////////////////////// CPU Code ////////////////////////////////////////////////////////////
//
std::string cls("\033[2J\033[H");
bool displayOn = true;

char PrintAndListenForQuit(ModelRunner<NeuronOperation, NeuronImplementation, NeuronModelHelper<CpuModelCarrier>, NeuronRecord>& modelRunner, CpuModelCarrier& carrier);
void PrintNeuronScan(ModelRunner<NeuronOperation, NeuronImplementation, NeuronModelHelper<CpuModelCarrier>, NeuronRecord>& modelRunner, CpuModelCarrier& carrier);
char MapIntensity(int activation);
void ParseArguments(int argc, char* argv[]);

void PrintSynapses(vector<NeuronNode>& model)
{
    for (auto i = 0; i < 50 * 25; i++)
    {
        cout << "Neuron " << i << " synapses:";

        auto& neuron = model[i];
        for (auto connection : neuron.PostsynapticConnections)
        {
            cout << " " << connection.PostsynapticNeuron << "/" << connection.Synapse;
        }
        cout << '\n';
    }
}

void TestBmtkLoading(ConfigurationRepository& configuration)
{
    constexpr const char* configurationFilePath = "/home/louis/source/bmtk/workspace/chapter04/sim_ch04/simulation_config.json";

#if false   // TODO - fix persister
    vector<NeuronNode> model {};
    CpuModelCarrier carrier(model, "./SensorInputFile.so");
    SonataModelRepository sonataRepository(configurationFilePath);
    SonataModelPersister persister(configurationFilePath, sonataRepository);
    persister.LoadConfiguration();
    persister.ReadModel(carrier, configuration);

    SonataInputSpikeLoader spikeLoader(persister.SonataRepository());
    spikeLoader.LoadSpikes(500);  // Is this number available in the LGN part of the configuration?
#endif
}

///////////////////////////////////////////////////////////////////////////
//Main program entry.
//Run the brain map.
//
int main(int argc, char* argv[])
{
    ParseArguments(argc, argv);
    vector<NeuronNode> model;
    ModelRunner<NeuronOperation, NeuronImplementation, NeuronModelHelper<CpuModelCarrier>, NeuronRecord> modelRunner(argc, argv);
    //TestBmtkLoading(modelRunner.ConfigurationCarrier());


    cout << "Discovering sensor input library\n";
    string sensorInputLibraryPath {};
    auto& executionSection = modelRunner.Configuration()["Execution"];
    if (!executionSection.is_null() && executionSection.contains("InputStreamer"))
    {
        cout << "Found 'Execution' section in configuration\n";
        auto& sensorInputLibrary = executionSection["InputStreamer"];
        cout << "Found 'InputStreamer' section in section\n";
        if (sensorInputLibrary.is_string())
            sensorInputLibraryPath = sensorInputLibrary.get<string>();
    }
    cout << "Using sensor input library" << sensorInputLibraryPath << "\n";

    CpuModelCarrier carrier(model, sensorInputLibraryPath);
    NeuronModelHelper<CpuModelCarrier> helper(carrier, modelRunner.ConfigurationCarrier());
    if (!modelRunner.Run(helper))
    {
        cout << "Cannot run model, stopping\n";
        return 1;
    }

    PrintAndListenForQuit(modelRunner, carrier);

    modelRunner.WaitForQuit();
    return 0;
}

char PrintAndListenForQuit(ModelRunner<NeuronOperation, NeuronImplementation, NeuronModelHelper<CpuModelCarrier>, NeuronRecord>& modelRunner, CpuModelCarrier& carrier)
{
    char c;
    {
        auto listener = KeyListener();

        bool quit {false};
        while (!quit)
        {
            if (displayOn) PrintNeuronScan(modelRunner, carrier);
            quit = listener.Listen(50'000, c);
        }
    }

    cout << "Received keystroke " << c << ", quitting\n";
    return c;
}

void PrintNeuronScan(ModelRunner<NeuronOperation, NeuronImplementation, NeuronModelHelper<CpuModelCarrier>, NeuronRecord>& modelRunner, CpuModelCarrier& carrier)
{
    cout << cls;

    auto node = begin(carrier.Model);
    for (auto high = 25; high; --high)
    {
        for (auto wide = 50; wide; --wide)
        {
            cout << MapIntensity(node->Activation);
            node++;
        }
        cout << '\n';
    }

    cout << "Iterations: " << modelRunner.GetModelEngine().GetIterations() << "  Total work: " << modelRunner.GetModelEngine().GetTotalWork() << "                 \n";
}

char MapIntensity(int activation)
{
    static int cutoffs[] = {2,5,15,50};

    if (activation < cutoffs[0]) return ' ';
    if (activation < cutoffs[1]) return '.';
    if (activation < cutoffs[2]) return '*';
    if (activation < cutoffs[3]) return 'o';
    return 'O';
}

void ParseArguments(int argc, char* argv[])
{
    for (auto i = 0; i < argc; i++)
    {
        string arg = argv[i];
        if (arg == "-d" || arg == "--nodisplay")
        {
            displayOn = false;
            cout << "Found -d flag, turning display off \n";
        }
    }
}