IDIR=../../include
ILDIR=../include
IH5DIR=/usr/include/hdf5/serial/
CC=g++
CCFLAGS=-I$(IDIR) -I$(ILDIR) -I$(IH5DIR) -fpic -pthread -std=c++17
LFLAGS=-fpic -pthread -std=c++17
LOG=-DNOLOG

ODIR=obj
BDIR=../bin
LDIR=../lib

LIBS= -ldl -lhdf5_serial -ltbb


_DEPS = ModelEngineCommon.h ModelEngineContext.h ModelEngineContextOp.h ModelEngine.h ModelEngineThread.h IModelEnginePartitioner.h AdaptiveWidthPartitioner.h ConstantWidthPartitioner.h IModelEngineWaiter.h ConstantTickWaiter.h FirstWorkWaiter.h WorkerContext.h WorkerContextOp.h Worker.h WorkerThread.h ProcessCallback.h Log.h Recorder.h sdk/ModelRunner.h sdk/IModelInitializer.h sdk/ModelInitializer.h sdk/ModelInitializerProxy.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_NEURONDEPS = NeuronCommon.h NeuronConnection.h NeuronImplementation.h NeuronModelHelper.h NeuronNode.h NeuronOperation.h NeuronRecord.h NeuronSynapse.h CpuModelCarrier.h
NEURONDEPS = $(patsubst %,$(ILDIR)/%,$(_NEURONDEPS))

_INITIALIZERDEPS = ModelNeuronInitializer.h ModelAnticipateInitializer.h ModelLayerInitializer.h ModelSonataInitializer.h
INITIALIZERDEPS = $(patsubst %,./%,$(_INITIALIZERDEPS))

_SENSORINPUTCOREDEPS = SensorInput/ISensorInput.h SensorInput/SensorInputProxy.h 
SENSORINPUTCOREDEPS = $(patsubst %,$(ILDIR)/%,$(_SENSORINPUTCOREDEPS))

_SENSORINPUTDEPS = SensorInputFile.h SensorSonataFile.h
SENSORINPUTDEPS = $(patsubst %,./%,$(_SENSORINPUTDEPS))

_SONATADEPS = persistence/sonata/SonataCompiledModel.h persistence/sonata/SonataEdgeModel.h persistence/sonata/SonataEdgeModelLoader.h persistence/sonata/SonataInputSpikeLoader.h persistence/sonata/SonataModelLoader.h persistence/sonata/SonataModelPersister.h persistence/sonata/SonataModelRepository.h persistence/sonata/SonataNodeModel.h persistence/sonata/SonataNodeModelLoader.h
SONATADEPS = $(patsubst %,$(ILDIR)/%,$(_SONATADEPS))

_LOCALDEPS = KeyListener.h
LOCALDEPS = $(patsubst %,./%,$(_LOCALDEPS))

$(ODIR)/%.o: %.cpp $(DEPS) $(NEURONDEPS) $(INITIALIZERDEPS) $(SENSORINPUTCOREDEPS) $(SENSORINPUTDEPS) $(SONATADEPS) $(LOCALDEPS)
	$(CC) -c -o $@ $< $(CCFLAGS) $(LOG)

all: $(BDIR)/NeuronModel $(BDIR)/ModelLayerInitializer.so $(BDIR)/ModelAnticipateInitializer.so $(BDIR)/ModelSonataInitializer.so $(BDIR)/SensorInputFile.so $(BDIR)/SensorSonataFile.so
.PHONY: all

log:
	make LOG=""

.PHONY: log

$(BDIR)/NeuronModel: $(ODIR)/NeuronModel.o
	$(CC) -o $@ $^ $(LFLAGS) $(LIBS)

$(BDIR)/ModelAnticipateInitializer.so: $(ODIR)/ModelAnticipateInitializer.o
	$(CC) -shared -o $@ $^ $(LFLAGS) $(LIBS)

$(BDIR)/ModelLayerInitializer.so: $(ODIR)/ModelLayerInitializer.o
	$(CC) -shared -o $@ $^ $(LFLAGS) $(LIBS)

$(BDIR)/ModelSonataInitializer.so: $(ODIR)/ModelSonataInitializer.o
	$(CC) -shared -o $@ $^ $(LFLAGS) $(LIBS)

$(BDIR)/SensorInputFile.so:  $(ODIR)/SensorInputFile.o
	$(CC) -shared -o $@ $^ $(LFLAGS) $(LIBS)

$(BDIR)/SensorSonataFile.so:  $(ODIR)/SensorSonataFile.o
	$(CC) -shared -o $@ $^ $(LFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
