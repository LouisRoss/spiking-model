#!/usr/bin/python3

import sys
import subprocess
from pathlib import Path

from mes import configuration
import nnclean
import nnplot
import nntidy

def load_configuration():
    conf = configuration()
    if 'PostProcessing' not in conf.configuration:
        print('Required "PostProcessing" section not in configuration file')
        return

    if 'RecordLocation' not in conf.configuration['PostProcessing']:
        print('Required subkey "RecordLocation" not in configuration file "PostProcessing" section')
        return

    record_path = conf.configuration['PostProcessing']['RecordLocation']
    Path(record_path).mkdir(parents=True, exist_ok=True)
    return conf

def parse_arguments(conf):
    argv = []

    if 'Execution' not in conf.configuration:
        print('Required "Execution" section not in configuration file')
        return

    if 'ModelExecutable' not in conf.configuration['Execution']:
        print('Required subkey "ModelExecutable" not in configuration file "Execution" section')
        return

    argv.append(conf.configuration['Execution']['ModelExecutable'])

    options = None
    if ('ModelOptions' in conf.configuration['Execution']):
        options = conf.configuration['Execution']['ModelOptions']

    if (options != None):
        print(f"Adding option '{options}'")
        argv.append(options)

    argv.append(sys.argv[1])
    return argv

def run():
    conf = load_configuration()
    argv = parse_arguments(conf)
    subprocess.run(argv)
    
    nnclean.execute(conf)   # Clean the record data into a clean file.
    nnplot.execute(conf)    # Plot the cleaned data into a file.
    nntidy.execute(conf)    # Archive the record data and plot(s).

run()
