import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

from mes import configuration

''' Using the cleaned record file (see mem.py),
    Add data to a line plot and save it to a file.
'''

def plot_cleaned_run(configuration):
    if 'PostProcessing' not in configuration.configuration:
        print ('Required "PostProcessing" section not in configuration file, unable to read cleaned run file')
        return

    if 'CleanRecordFile' not in configuration.configuration['PostProcessing']:
        print ('Required key "CleanRecordFile" not in configuration file "PostProcessing" section, unable to read cleaned run file')
        return
            
    filename = configuration.configuration['PostProcessing']['CleanRecordFile']
    clean_record_file_path = configuration.find_projectpath()
    clean_record_file_path = clean_record_file_path + filename
    print("Using clean record file '" + clean_record_file_path + "'")

    plot_from_file(configuration, clean_record_file_path, 'Full Run')

def plot_epochs(configuration):
    epochs_file_path = configuration.find_projectpath()
    projectdir = Path(epochs_file_path)
    for epoc_data_path in projectdir.glob('epoch*csv'):
        plot_from_file(configuration, str(epoc_data_path), str(epoc_data_path).split('/')[-1])

def plot_from_file(configuration, filepath, title):
    neurons = pd.read_csv(filepath, index_col=0)
    show_lineplot(neurons, filepath, title)

def show_lineplot(neurons, filepath, title):
    columns = neurons.columns
    x_data = range(0, neurons.shape[0])
    #plt.style.use('Solarize_Light2')
    fig, ax = plt.subplots(figsize=(22,12))
    for column in columns:
        ax.plot(x_data, neurons[column], label=column)
    ax.set_title(title)
    ax.legend()
    #plt.show()
    image_file_path = filepath.replace('.csv', '.png')
    print("Writing image file '" + image_file_path + "'")
    fig.savefig(image_file_path, dpi=100, bbox_inches='tight')
    plt.close('all')

def execute(configuration):
    plot_cleaned_run(configuration)
    plot_epochs(configuration)

def run():
    conf = configuration()
    execute(conf)

if __name__ == "__main__":
    run()
