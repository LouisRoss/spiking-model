IDIR=../../include
ILDIR=../include
IH5DIR=/usr/include/hdf5/serial/
CC=g++
CFLAGS=-I$(IDIR) -I$(ILDIR) -I$(IH5DIR) -pthread -std=c++17

ODIR=obj
BDIR=../../bin
LDIR=../../lib

LIBS= -ldl -lhdf5_serial -ltbb


_DEPS = ModelEngineCommon.h ModelEngineContext.h ModelEngineContextOp.h ModelEngine.h ModelEngineThread.h IModelEnginePartitioner.h AdaptiveWidthPartitioner.h ConstantWidthPartitioner.h IModelEngineWaiter.h ConstantTickWaiter.h FirstWorkWaiter.h WorkerContext.h WorkerContextOp.h Worker.h WorkerThread.h ProcessCallback.h Log.h Recorder.h sdk/ModelRunner.h sdk/IModelInitializer.h sdk/ModelInitializer.h sdk/ModelInitializerProxy.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_NEURONDEPS = NeuronCommon.h NeuronConnection.h NeuronImplementation.h NeuronModelHelper.h NeuronNode.h NeuronOperation.h NeuronRecord.h NeuronSynapse.h
NEURONDEPS = $(patsubst %,$(ILDIR)/%,$(_NEURONDEPS))

_SONATADEPS = persistence/sonata/SonataCompiledModel.h persistence/sonata/SonataEdgeModel.h persistence/sonata/SonataEdgeModelLoader.h persistence/sonata/SonataInputSpikeLoader.h persistence/sonata/SonataModelLoader.h persistence/sonata/SonataModelPersister.h persistence/sonata/SonataModelRepository.h persistence/sonata/SonataNodeModel.h persistence/sonata/SonataNodeModelLoader.h
SONATADEPS = $(patsubst %,$(ILDIR)/%,$(_SONATADEPS))

_LOCALDEPS = KeyListener.h
LOCALDEPS = $(patsubst %,./%,$(_LOCALDEPS))

_OBJ = NeuronModel.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS) $(NEURONDEPS) $(SONATADEPS) $(LOCALDEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BDIR)/NeuronModel: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~
