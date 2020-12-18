import copy
import time
import csv
import re
from enum import Enum
from pathlib import Path

from mes import configuration

'''
Extract the configured channels from the record CSV file
and write a new cleaned CSV file with a unique column for
each of the configured channels.
'''
class NeuronRecordType(Enum):
    InputSignal = 0
    Decay = 1
    Spike = 2
    Refractory = 3


class Cleaner:
    configuration = None
    output = []
    epochoutput = []
    outputheader = []
    outputrow = []
    trigger = None

    def __init__(self, configuration):
        ''' Create the header for the cleaned CSV file, with the neuron's
            name and numeric ID for column head.  Clean the output row
            to contain the correct number of int values in preparation.
        '''
        self.configuration = configuration

        if 'MonitorNeurons' not in self.configuration.monitor:
            print('Required "MonitorNeurons" section not in monitor file')
            return

        self.outputheader.append('time')
        self.outputrow.append(0)

        monitor_neurons = self.configuration.monitor['MonitorNeurons']
        for neuron in monitor_neurons:
            name = neuron[0] + '(' + str(neuron[1]) + ')'
            self.outputheader.append(name)
            self.outputrow.append(0)

        self.output.append(copy.copy(self.outputheader))
        self.epochoutput.append(copy.copy(self.outputheader))

    def clean_data(self):
        ''' Open the record CSV file per the configuration.  Read through
            the whole record file, extracting the configured channels,
            and writing them into the cleaned CSV file using a column-per-channel
            format.
        '''
        if 'PostProcessing' not in self.configuration.configuration:
            print ('Required "PostProcessing" section not in configuration file, unable to open record file')
            return

        if 'RecordFile' not in self.configuration.configuration['PostProcessing']:
            print ('Required key "RecordFile" not in configuration file "PostProcessing" section, unable to open record file')
            return
            
        if 'MonitorNeurons' not in self.configuration.monitor:
            print ('Required "PostProcessing" section not in monitor file, unable to clean record file')
            return

        record_path = self.configuration.find_projectpath()
        record_path = record_path.rstrip('/') + '/' + self.configuration.configuration['PostProcessing']['RecordFile']
        print("Cleaning record file '" + record_path + "'")
        f = open(record_path)
        csv_f = csv.reader(f)

        monitor_neurons = self.configuration.monitor['MonitorNeurons']
        monitor_indexes = [monitor_neurons[i][1] for i in range(0, len(monitor_neurons))]

        last_tick = 0
        last_row = []
        header = []

        self.read_trigger()

        for row in csv_f:
            if row[0] == 'tick':
                header = copy.copy(row)
            else:
                tick = int(row[header.index('tick')])
                event_type = int(row[header.index('Neuron-Event-Type')])
                if event_type == NeuronRecordType.InputSignal.value or event_type == NeuronRecordType.Refractory.value or event_type == NeuronRecordType.Decay.value:
                    active_neuron = int(row[header.index('Neuron-Index')])
                    if active_neuron in monitor_indexes:
                        last_row = copy.copy(self.outputrow)
                        if last_tick != 0 and tick != last_tick:
                            self.fill_output(last_row, last_tick, tick)
                        self.outputrow[monitor_indexes.index(active_neuron) + 1] = row[header.index('Neuron-Activation')]
                        last_tick = tick

                    if self.is_trigger(header, row):
                        self.write_cleaned_epoch()

        self.write_cleaned_run()

    def fill_output(self, row, first_tick, last_tick):
        ''' Extend a straigt-line value from first_tick through last_tick,
            ensuring that the cleaned output file has a value for all ticks.
        '''
        tick = first_tick
        row[0] = tick
        self.output.append(copy.copy(row))
        self.epochoutput.append(copy.copy(row))

        tick += 1
        while tick < last_tick:
            row[0] = tick
            self.output.append(copy.copy(row))
            self.epochoutput.append(copy.copy(row))
            tick += 1

    def read_trigger(self):
        if 'TriggerEvent' in self.configuration.monitor:
            self.trigger = self.configuration.monitor['TriggerEvent']

    def is_trigger(self, header, row):
        if self.trigger == None:
            return False

        if self.trigger['EventType'] == NeuronRecordType.InputSignal.value:
            return int(row[header.index('Neuron-Event-Type')]) == NeuronRecordType.InputSignal.value and \
            int(row[header.index('Neuron-Index')]) == self.trigger['NeuronIndex'] and                         \
            int(row[header.index('Synapse-Index')]) == self.trigger['SynapseIndex']

        elif self.trigger['EventType'] == NeuronRecordType.Spike.value:
            return int(row[header.index('Neuron-Event-Type')]) == NeuronRecordType.Spike.value and       \
            int(row[header.index('Neuron-Index')]) == self.trigger['NeuronIndex']

        return False

    def write_cleaned_run(self):
        ''' Write the cleaned data for the entire run to the configured clean output file.
        '''
        if 'PostProcessing' not in self.configuration.configuration:
            print ('Required "PostProcessing" section not in configuration file, unable to write cleaned run file')
            return

        if 'CleanRecordFile' not in self.configuration.configuration['PostProcessing']:
            print ('Required key "CleanRecordFile" not in configuration file "PostProcessing" section, unable to write cleaned run file')
            return
            
        project_path = self.configuration.find_projectpath()
        record_path = project_path.rstrip('/') + '/' + self.configuration.configuration['PostProcessing']['CleanRecordFile']
        print("Writing clean record file '" + record_path + "'")
        with open(record_path, mode='w') as clean_data:
            data_writer = csv.writer(clean_data, delimiter=',', quoting=csv.QUOTE_NONE)
            data_writer.writerows(self.output)

    def write_cleaned_epoch(self):
        ''' Write the cleaned epoch data to the configured clean output file.
            Note this may be called when the first epoch starts, so skip that one.
        '''
        if len(self.epochoutput) > 2:
            for i in range(1, len(self.epochoutput)):
                self.epochoutput[i][0] = i           # Tick starts with 1 for all epochs.
            project_path = self.configuration.find_projectpath()
            epoch_number = self.get_epoch_number_from_project(project_path)
            record_path = project_path.rstrip('/') + '/' + "epoch" + str(epoch_number) + ".csv"
            print("Writing clean epoch file '" + record_path + "'")
            with open(record_path, mode='w') as clean_data:
                data_writer = csv.writer(clean_data, delimiter=',', quoting=csv.QUOTE_NONE)
                data_writer.writerows(self.epochoutput)
        
        self.epochoutput = []   # Clear the epoch data, starting over with just a header row.
        self.epochoutput.append(copy.copy(self.outputheader))


    def get_epoch_number_from_project(self, projectpath):
        ''' Examine all the existing 'epochN.csv' files, looking
            for the largest N.  Return N+1.
        '''
        projectdir = Path(projectpath)
        projectdir.mkdir(exist_ok=True)

        largest_epoch = 0
        epochs_in_project = (entry for entry in projectdir.glob('epoch*csv'))
        for epoch in epochs_in_project:
            epoch_name = str(epoch).split('/')[-1]
            epoch_number = int(re.search(r"([a-zA-Z]*)(\d*)", epoch_name).group(2))
            if (epoch_number > largest_epoch):
                largest_epoch = epoch_number

        return largest_epoch + 1


def execute(configuration):
    cleaner = Cleaner(configuration)
    cleaner.clean_data()

def run():
    conf = configuration()
    execute(conf)

if __name__ == "__main__":
    run()
