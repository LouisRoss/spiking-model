IDIR=../../../include
ILDIR=../../include
IH5DIR=/usr/include/hdf5/serial/
CC=g++
CCFLAGS=-I$(IDIR) -I$(ILDIR) -I$(IH5DIR) -fpic -pthread -std=c++17
LFLAGS=-fpic -pthread -std=c++17
LOG=-DNOLOG
DEBUGFLAG=

ODIR=obj
BDIR=../../bin
LDIR=../../lib

LIBS= -ldl -lhdf5_serial -ltbb


_DEPS = ModelEngineCommon.h ModelEngineContext.h ModelEngineContextOp.h ModelEngine.h ModelEngineThread.h IModelEnginePartitioner.h AdaptiveWidthPartitioner.h ConstantWidthPartitioner.h IModelEngineWaiter.h ConstantTickWaiter.h FirstWorkWaiter.h WorkerContext.h WorkerContextOp.h Worker.h WorkerThread.h ProcessCallback.h Log.h Recorder.h sdk/ModelRunner.h sdk/ModelInitializerProxy.h sdk/SensorInputProxy.h sdk/KeyListener.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_NEURONDEPS = NeuronCommon.h NeuronConnection.h NeuronImplementation.h NeuronModelHelper.h NeuronNode.h NeuronOperation.h NeuronRecord.h NeuronSynapse.h CpuModelCarrier.h
NEURONDEPS = $(patsubst %,$(ILDIR)/%,$(_NEURONDEPS))

_INITIALIZERDEPS = IModelInitializer.h ModelAnticipateInitializer.h ModelInitializer.h ModelLayerInitializer.h ModelNeuronInitializer.h ModelSonataInitializer.h
INITIALIZERDEPS = $(patsubst %,$(IDIR)/Initializers/%,$(_INITIALIZERDEPS))

_SONATADEPS = persistence/sonata/SonataCompiledModel.h persistence/sonata/SonataEdgeModel.h persistence/sonata/SonataEdgeModelLoader.h persistence/sonata/SonataInputSpikeLoader.h persistence/sonata/SonataModelLoader.h persistence/sonata/SonataModelPersister.h persistence/sonata/SonataModelRepository.h persistence/sonata/SonataNodeModel.h persistence/sonata/SonataNodeModelLoader.h
SONATADEPS = $(patsubst %,$(ILDIR)/%,$(_SONATADEPS))

all: $(BDIR)/ModelAnticipateInitializer.so $(BDIR)/ModelLayerInitializer.so $(BDIR)/ModelSonataInitializer.so 
.PHONY: all

$(BDIR)/ModelAnticipateInitializer.so: $(ODIR)/ModelAnticipateInitializer.o
	$(CC) -shared -o $@ $^ $(DEBUGFLAG) $(LFLAGS) $(LIBS)

$(BDIR)/ModelLayerInitializer.so: $(ODIR)/ModelLayerInitializer.o
	$(CC) -shared -o $@ $^ $(DEBUGFLAG) $(LFLAGS) $(LIBS)

$(BDIR)/ModelSonataInitializer.so: $(ODIR)/ModelSonataInitializer.o
	$(CC) -shared -o $@ $^ $(DEBUGFLAG) $(LFLAGS) $(LIBS)

$(ODIR)/%.o: %.cpp $(DEPS) $(NEURONDEPS) $(INITIALIZERDEPS) $(SONATADEPS)
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(DEBUGFLAG) $(CCFLAGS) $(LOG)

debug:
	make DEBUGFLAG="-g"
.PHONY: debug

log:
	make LOG=""
.PHONY: log

clean:
	rm -f $(ODIR)/*.o
.PHONY: clean
