import shutil
import os
import re
from pathlib import Path

from mes import configuration

def find_projectpath(configuration):
    project_name = configuration.control["Configuration"]
    project_name = project_name.split('/')[-1].rstrip('.json')
    projectpath = configuration.configuration['PostProcessing']['RecordLocation'].rstrip('/') + '/' + project_name + '/'
    print('Using project name ' + project_name + ", and record path " + projectpath)
    Path(projectpath).mkdir(parents=True, exist_ok=True)
    return projectpath

def get_runnumber_from_project(projectpath):
    projectdir = Path(projectpath)
    projectdir.mkdir(exist_ok=True)

    largest_run = 0
    runs_in_project = (entry for entry in projectdir.iterdir() if entry.is_dir())
    for directory in runs_in_project:
        run = str(directory).split('/')[-1]
        run_number = int(re.search(r"([a-zA-Z]*)(\d*)", run).group(2))
        if (run_number > largest_run):
            largest_run = run_number

    return largest_run + 1

def archive_run(configuration, projectpath, runnumber):
    runpath = projectpath + 'run' + str(runnumber)
    rundir = Path(runpath)
    rundir.mkdir()

    recordpath = configuration.configuration['PostProcessing']['RecordLocation']
    recordfile = configuration.configuration['PostProcessing']['RecordFile']
    imagefile = configuration.configuration['PostProcessing']['ImageFile']
    shutil.move(recordpath + recordfile, runpath)
    shutil.move(recordpath + imagefile, runpath)
    print('Moved ' + recordfile + ' and ' + imagefile + ' to new run dir ' + runpath)
    cleanrecordfile = configuration.configuration['PostProcessing']['CleanRecordFile']
    os.remove(recordpath + cleanrecordfile)
    print('Deleted ' + cleanrecordfile)

def execute(configuration):
    projectpath = find_projectpath(configuration)
    runnumber = get_runnumber_from_project(projectpath)
    print('Next run: ' + str(runnumber))
    archive_run(configuration, projectpath, runnumber)

def run():
    conf = configuration()
    execute(conf)

if __name__ == "__main__":
    run()
