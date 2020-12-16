import sys
import json
from pathlib import Path

class configuration:
    settings = None
    control = None
    configuration = None
    monitor = None
    settings_path = 'ModelSettings.json'
    configuration_path = './'
    control_file = None

    def __init__(self):
        if not self.parse_arguments():
            return
        self.load_settings()
        self.load_control()
        self.load_configuration()
        self.load_monitor()

    def parse_arguments(self):
        if len(sys.argv) < 2:
            print('Usage: mes <controlfile> arg1 arg2 ...')
            return False

        self.control_file = sys.argv[1]
        if not self.control_file.endswith('.json'):
            self.control_file += '.json'
        return True
        
    def load_settings(self):
        with open(self.settings_path) as f:
            self.settings = json.load(f)

        self.configuration_path = self.settings['ConfigFilePath']
        self.configuration_path = self.configuration_path.rstrip('/') + '/'

    def load_control(self):
        control_full_path = self.configuration_path + self.control_file
        with open(control_full_path) as f:
            self.control = json.load(f)
        print("Using control path '" + control_full_path + "'")

    def load_configuration(self):
        config_file = self.control['Configuration']
        if not config_file.endswith('.json'):
            config_file += '.json'
        configuration_full_path = self.configuration_path + config_file
        with open(configuration_full_path) as f:
            self.configuration = json.load(f)
        print("Using configuration path '" + configuration_full_path + "'")

    def load_monitor(self):
        monitor_file = self.control['Monitor']
        if not monitor_file.endswith('.json'):
            monitor_file += '.json'
        monitor_full_path = self.configuration_path + monitor_file
        with open(monitor_full_path) as f:
            self.monitor = json.load(f)
        print("Using monitor path '" + monitor_full_path + "'")

    def find_projectpath(self):
        if 'PostProcessing' not in self.configuration:
            print ('Required "PostProcessing" section not in configuration file, unable to find project path')
            return './'

        if 'CleanRecordFile' not in self.configuration['PostProcessing']:
            print ('Required key "CleanRecordFile" not in configuration file "PostProcessing" section, unable to find project path')
            return './'

        project_name = self.control["Configuration"]
        project_name = project_name.split('/')[-1].rstrip('.json')
        projectpath = self.configuration['PostProcessing']['RecordLocation'].rstrip('/') + '/' + project_name + '/'
        print('Using project name ' + project_name + ", and record path " + projectpath)
        Path(projectpath).mkdir(parents=True, exist_ok=True)
        return projectpath
