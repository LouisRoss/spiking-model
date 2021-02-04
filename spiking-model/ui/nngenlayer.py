#!/usr/bin/python3

import json
import collections
from pathlib import Path

from mes import configuration

class LayerGenerator:
    configuration = None
    signal_to_inject = {}

    def __init__(self, configuration):
        ''' Confirm all required configuration elements are present in the
            configuration, and make sure the path to the record location exists.
        '''
        self.configuration = configuration

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

    def generate_layer(self):
        self.signal_to_inject = {}
        for i in range(1,6):
            self.insert_signal(i*3+0, 1)
            self.insert_signal(i*3-1, 2)
            self.insert_signal(i*3-2, 3)
            self.insert_signal(i*3-3, 4)
            self.insert_signal(i*3-4, 5)

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
    generator = LayerGenerator(configuration)
    generator.generate_layer()
    generator.write_streaming_input_file()

def run():
    conf = configuration()
    execute(conf)

if __name__ == "__main__":
    run()
