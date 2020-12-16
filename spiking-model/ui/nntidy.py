import shutil
import os
import re
from pathlib import Path

from mes import configuration

''' Archive any working data (record file, plotted data)
    from the working folder into a run folder in the 
    configured record folder.
'''

def get_runnumber_from_project(projectpath):
    ''' Examine all the existing 'runN' directories, looking
        for the largest N.  Return N+1.
    '''
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

    projectdir = Path(projectpath)    
    data_files_in_project = (entry for entry in projectdir.glob('*csv'))
    for file in data_files_in_project:
        shutil.move(str(file), runpath)

    image_files_in_project = (entry for entry in projectdir.glob('*png'))
    for file in image_files_in_project:
        shutil.move(str(file), runpath)

    log_files_in_project = (entry for entry in projectdir.glob('*log'))
    for file in log_files_in_project:
        shutil.move(str(file), runpath)

    print('Moved all data and image files to new run dir ' + runpath)

def execute(configuration):
    projectpath = configuration.find_projectpath()
    runnumber = get_runnumber_from_project(projectpath)
    print('Next run: ' + str(runnumber))
    archive_run(configuration, projectpath, runnumber)

def run():
    conf = configuration()
    execute(conf)

if __name__ == "__main__":
    run()
