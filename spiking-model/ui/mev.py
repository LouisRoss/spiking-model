import pandas as pd
import matplotlib.pyplot as plt

from mes import configuration

def show_lineplot(configuration):
    record_path = configuration.configuration['PostProcessing']['RecordLocation']
    record_path = record_path.rstrip('/') + '/' + configuration.configuration['PostProcessing']['CleanRecordFile']
    print("Using clean record file '" + record_path + "'")
    neurons = pd.read_csv(record_path, index_col=0)

    columns = neurons.columns
    x_data = range(0, neurons.shape[0])
    #plt.style.use('Solarize_Light2')
    fig, ax = plt.subplots(figsize=(22,12))
    for column in columns:
        ax.plot(x_data, neurons[column], label=column)
    ax.set_title('Neuron Activation')
    ax.legend()
    #plt.show()
    record_path = configuration.configuration['PostProcessing']['RecordLocation']
    record_file = configuration.configuration['PostProcessing']['ImageFile']
    record_path = record_path.rstrip('/') + '/' + record_file
    print("Writing image file '" + record_path + "'")
    fig.savefig(record_path, dpi=100, bbox_inches='tight')

def execute(configuration):
    show_lineplot(configuration)

def run():
    conf = configuration()
    execute(conf)

if __name__ == "__main__":
    run()
