#!/usr/bin/python3

import json
import collections
from pathlib import Path

from mes import configuration

# TODO make this configurable or in some other repository.
signal_delay_time = 7

class AnticipateGenerator:
    configuration = None
    signal_to_inject = {}
    width = 50
    height = 25

    def __init__(self, configuration):
        ''' Confirm all required configuration elements are present in the
            configuration, and make sure the path to the record location exists.
        '''
        self.configuration = configuration

        if 'Model' in self.configuration.configuration:
            if 'Dimensions' in self.configuration.configuration['Model']:
                dimensions = self.configuration.configuration['Model']['Dimensions']
                self.width = dimensions[0]
                self.height = dimensions[1]

        if 'PostProcessing' not in self.configuration.configuration:
            print('Required "PostProcessing" section not in configuration file')
            return

        if 'RecordLocation' not in self.configuration.configuration['PostProcessing']:
            print('Required subkey "RecordLocation" not in configuration file "PostProcessing" section')
            return

        if 'SensorInputFile' not in self.configuration.configuration['PostProcessing']:
            print('Required subkey "SensorInputFile" not in configuration file "PostProcessing" section')
            return

        record_path = self.configuration.configuration['PostProcessing']['RecordLocation']
        Path(record_path).mkdir(parents=True, exist_ok=True)

    def get_sensor_input_file_path(self):
        ''' Develop the full file path to the sensor input file,
            placing it in the project path of the record path.
        '''
        project_path = self.configuration.find_projectpath()
        file_name = self.configuration.configuration['PostProcessing']['SensorInputFile']
        return project_path + file_name

    def generate_anticipate(self, configuration):
        global signal_delay_time

        self.signal_to_inject = {}
        i1_index = configuration.get_neuron_index("I1")
        i2_index = configuration.get_neuron_index("I2")

        for i in range(0, 8000, 200):
            self.insert_signal(i, i1_index)
            self.insert_signal(i+(signal_delay_time*4)+2, i2_index)

        self.signal_to_inject = collections.OrderedDict(sorted(self.signal_to_inject.items()))

    def insert_signal(self, tick, index):
        if tick not in self.signal_to_inject:
            self.signal_to_inject[tick] = []
        self.signal_to_inject[tick].append(index)

    def write_streaming_input_file(self):
        file_path = self.get_sensor_input_file_path()
        print('writing streaming input file to ' + file_path)
        with open(file_path, 'w', encoding='utf-8') as f:
            json.dump(self.signal_to_inject, f, ensure_ascii=False, indent=4)

def execute(configuration):
    generator = AnticipateGenerator(configuration)
    generator.generate_anticipate(configuration)
    generator.write_streaming_input_file()

def run():
    conf = configuration()
    execute(conf)

if __name__ == "__main__":
    run()
