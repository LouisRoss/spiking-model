IDIR=../../../include
ILDIR=../../include
IH5DIR=/usr/include/hdf5/serial/
CC=g++
CCFLAGS=-I$(IDIR) -I$(ILDIR) -I$(IH5DIR) -fpic -pthread -std=c++17
LFLAGS=-fpic -pthread -std=c++17
LOG=-DNOLOG

ODIR=obj
BDIR=../../bin
LDIR=../../lib

LIBS= -ldl -lhdf5_serial -ltbb


_DEPS = ModelEngineCommon.h ModelEngineContext.h ModelEngineContextOp.h ModelEngine.h ModelEngineThread.h IModelEnginePartitioner.h AdaptiveWidthPartitioner.h ConstantWidthPartitioner.h IModelEngineWaiter.h ConstantTickWaiter.h FirstWorkWaiter.h WorkerContext.h WorkerContextOp.h Worker.h WorkerThread.h ProcessCallback.h Log.h Recorder.h sdk/ModelRunner.h sdk/ModelInitializerProxy.h sdk/SensorInputProxy.h sdk/KeyListener.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_NEURONDEPS = NeuronCommon.h NeuronConnection.h NeuronImplementation.h NeuronModelHelper.h NeuronNode.h NeuronOperation.h NeuronRecord.h NeuronSynapse.h CpuModelCarrier.h
NEURONDEPS = $(patsubst %,$(ILDIR)/%,$(_NEURONDEPS))

_SENSORINPUTDEPS = ISensorInput.h  SensorInputFile.h  SensorSonataFile.h
SENSORINPUTDEPS = $(patsubst %,$(IDIR)/SensorInputs/%,$(_SENSORINPUTDEPS))

_SONATADEPS = persistence/sonata/SonataCompiledModel.h persistence/sonata/SonataEdgeModel.h persistence/sonata/SonataEdgeModelLoader.h persistence/sonata/SonataInputSpikeLoader.h persistence/sonata/SonataModelLoader.h persistence/sonata/SonataModelPersister.h persistence/sonata/SonataModelRepository.h persistence/sonata/SonataNodeModel.h persistence/sonata/SonataNodeModelLoader.h
SONATADEPS = $(patsubst %,$(ILDIR)/%,$(_SONATADEPS))

all: $(BDIR)/SensorInputFile.so $(BDIR)/SensorSonataFile.so 
.PHONY: all

$(BDIR)/SensorInputFile.so: $(ODIR)/SensorInputFile.o
	$(CC) -shared -o $@ $^ $(LFLAGS) $(LIBS)

$(BDIR)/SensorSonataFile.so: $(ODIR)/SensorSonataFile.o
	$(CC) -shared -o $@ $^ $(LFLAGS) $(LIBS)

$(ODIR)/%.o: %.cpp $(DEPS) $(NEURONDEPS) $(SENSORINPUTDEPS) $(SONATADEPS)
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CCFLAGS) $(LOG)

log:
	make LOG=""
.PHONY: log

clean:
	rm -f $(ODIR)/*.o
.PHONY: clean
