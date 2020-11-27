#!/usr/bin/python3

import sys
import subprocess
from pathlib import Path

from mes import configuration
import mem as mem
import mev as mev
import mec as mec

def run():
    conf = configuration()
    record_path = conf.configuration['PostProcessing']['RecordLocation']
    Path(record_path).mkdir(parents=True, exist_ok=True)

    argv = []
    argv.append(conf.configuration['Execution']['ModelExecutable'])
    options = None
    if ('ModelOptions' in conf.configuration['Execution']):
        options = conf.configuration['Execution']['ModelOptions']
    if (options != None):
        print(f"Adding option '{options}'")
        argv.append(options)
    argv.append(sys.argv[1])
    subprocess.run(argv)
    
    mem.execute(conf)
    mev.execute(conf)
    mec.execute(conf)

run()
