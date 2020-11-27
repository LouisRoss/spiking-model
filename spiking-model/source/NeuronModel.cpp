#include <memory>
#include <iostream>
#include <string>
#include <chrono>
#include <stdio.h>
#include "ModelEngine.h"
#include "NeuronOperation.h"
#include "NeuronImplementation.h"
#include "NeuronNode.h"
#include "NeuronRecord.h"
#include "sdk/ModelRunner.h"
#include "persistence/sonata/SonataModelRepository.h"
#include "persistence/sonata/SonataModelPersister.h"
#include "persistence/sonata/SonataInputSpikeLoader.h"
#include "KeyListener.h"
#include "nlohmann/json.hpp"
#include "highfive/H5Easy.hpp"

using namespace std;
using namespace std::chrono;
using namespace nlohmann;

using embeddedpenguins::modelengine::ModelEngine;
using embeddedpenguins::modelengine::sdk::ModelRunner;
using embeddedpenguins::neuron::infrastructure::NeuronOperation;
using embeddedpenguins::neuron::infrastructure::NeuronImplementation;
using embeddedpenguins::neuron::infrastructure::NeuronNode;
using embeddedpenguins::neuron::infrastructure::NeuronRecord;
using embeddedpenguins::neuron::infrastructure::KeyListener;
using embeddedpenguins::neuron::infrastructure::persistence::sonata::SonataModelRepository;
using embeddedpenguins::neuron::infrastructure::persistence::sonata::SonataModelPersister;
using embeddedpenguins::neuron::infrastructure::persistence::sonata::SonataInputSpikeLoader;
using std::vector;
using std::chrono::nanoseconds;
using std::chrono::system_clock;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::ceil;

//////////////////////////////////////////////////////////// CPU Code ////////////////////////////////////////////////////////////
//
std::string cls("\033[2J\033[H");
bool displayOn = true;

char PrintAndListenForQuit(ModelRunner<NeuronNode, NeuronOperation, NeuronImplementation, NeuronRecord>& modelRunner);
void PrintNeuronScan(ModelRunner<NeuronNode, NeuronOperation, NeuronImplementation, NeuronRecord>& modelRunner);
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

void TestBmtkLoading()
{
    constexpr const char* configurationFilePath = "/home/louis/source/bmtk/workspace/chapter04/sim_ch04/simulation_config.json";

    vector<NeuronNode> model {};
    SonataModelRepository sonataRepository(configurationFilePath);
    SonataModelPersister persister(configurationFilePath, sonataRepository);
    persister.LoadConfiguration();
    persister.ReadModel(model);

    SonataInputSpikeLoader spikeLoader(persister.SonataRepository());
    spikeLoader.LoadSpikes(500);  // Is this number available in the LGN part of the configuration?
}

///////////////////////////////////////////////////////////////////////////
//Main program entry.
//Run the brain map.
//
int main(int argc, char* argv[])
{
    //TestBmtkLoading();

    ParseArguments(argc, argv);
    ModelRunner<NeuronNode, NeuronOperation, NeuronImplementation, NeuronRecord> modelRunner(argc, argv);
    if (!modelRunner.Run())
    {
        cout << "Cannot run model, stopping\n";
        return 1;
    }

    PrintAndListenForQuit(modelRunner);

    modelRunner.WaitForQuit();
    return 0;
}

char PrintAndListenForQuit(ModelRunner<NeuronNode, NeuronOperation, NeuronImplementation, NeuronRecord>& modelRunner)
{
    char c;
    {
        auto listener = KeyListener();

        bool quit {false};
        while (!quit)
        {
            if (displayOn) PrintNeuronScan(modelRunner);
            quit = listener.Listen(50'000, c);
        }
    }

    cout << "Received keystroke " << c << ", quitting\n";
    return c;
}

void PrintNeuronScan(ModelRunner<NeuronNode, NeuronOperation, NeuronImplementation, NeuronRecord>& modelRunner)
{
    cout << cls;

    auto node = begin(modelRunner.GetModel());
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