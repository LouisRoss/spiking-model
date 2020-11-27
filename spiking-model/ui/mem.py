import copy
import time
import csv
from enum import Enum

from mes import configuration

output = []
outputrow = []

class NeuronRecordType(Enum):
    InputSignal = 0
    Decay = 1
    Spike = 2
    Refractory = 3

def extract_tick(timestamp):
    t = time.strptime(timestamp[:19], "%Y-%m-%d %H:%M:%S")
    seconds = t.tm_sec
    nanoseconds = int(timestamp[20:])
    return int(round(((seconds * 10000) + (nanoseconds / 100000))))

def init_output(configuration):
    global output
    global outputrow
    monitor_neurons = configuration.monitor['MonitorNeurons']
    outputrow.append('time')
    for neuron in monitor_neurons:
        name = neuron[0] + '(' + str(neuron[1]) + ')'
        outputrow.append(name)
    output.append(copy.copy(outputrow))
    for i in range(0, len(outputrow)):
        outputrow[i] = 0

def fill_output(row, first_tick, last_tick):
    tick = first_tick
    row[0] = tick
    output.append(copy.copy(row))

    tick += 1
    while tick < last_tick:
        row[0] = tick
        output.append(copy.copy(row))
        tick += 1

def clean_data(configuration):
    record_path = configuration.configuration['PostProcessing']['RecordLocation']
    record_path = record_path.rstrip('/') + '/' + configuration.configuration['PostProcessing']['RecordFile']
    print("Creating clean record file from '" + record_path + "'")
    f = open(record_path)
    monitor_neurons = configuration.monitor['MonitorNeurons']
    csv_f = csv.reader(f)
    last_tick = 0
    last_row = []
    header = []
    monitor_indexes = [monitor_neurons[i][1] for i in range(0, len(monitor_neurons))]

    for row in csv_f:
        if row[0] == 'tick':
            header = copy.copy(row)
        else:
            tick = int(row[header.index('tick')])
            event_type = int(row[header.index('Neuron-Event-Type')])
            if event_type == NeuronRecordType.InputSignal.value or event_type == NeuronRecordType.Refractory.value or event_type == NeuronRecordType.Decay.value:
                active_neuron = int(row[header.index('Neuron-Index')])
                if active_neuron in monitor_indexes:
                    last_row = copy.copy(outputrow)
                    if last_tick != 0 and tick != last_tick:
                        fill_output(last_row, last_tick, tick)
                    outputrow[monitor_indexes.index(active_neuron) + 1] = row[header.index('Neuron-Activation')]
                    last_tick = tick

def write_data(configuration):
    record_path = configuration.configuration['PostProcessing']['RecordLocation']
    record_path = record_path.rstrip('/') + '/' + configuration.configuration['PostProcessing']['CleanRecordFile']
    print("Writing clean record file '" + record_path + "'")
    with open(record_path, mode='w') as clean_data:
        data_writer = csv.writer(clean_data, delimiter=',', quoting=csv.QUOTE_NONE)
        data_writer.writerows(output)

def execute(configuration):
    init_output(configuration)
    clean_data(configuration)
    write_data(configuration)

def run():
    conf = configuration()
    execute(conf)

if __name__ == "__main__":
    run()
