# This is a makefile intended to copy any files originally obtained
# from another repo back to those repos, so their changed status
# can be ascertained, and proper commits can be done there.
# Basically, files obtained this way are obtained using the 
# add-dependencies script, so this is intended to reverse those copies.
# As such, it is expected that the repositories we depend on are present
# in parallel folders to this one, and up to date before running this.
#
# use:
# >make -f backfill

COREDIR=../spiking-model-core

_COREINCS = NeuronRecordCommon.h 
COREINCS = $(patsubst %,$(COREDIR)/include/%,$(_COREINCS))

_CORECONFIGS = a1.json bmtk1.json l1.json Anticipate/Anticipate.json Anticipate/I1-I2-N1-N2.json BMTK1/bmtk.json BMTK1/2-5.json Layer/Layer.json Layer/Layer1-5.json
CORECONFIGS = $(patsubst %,$(COREDIR)/config/%,$(_CORECONFIGS))

_COREUIS = Makefile mec.py mem.py mes.py mev.py nnclean.py nngenanticipate.py nngenlayer.py nnplot.py nn.py nntidy.py np.py 
COREUIS = $(patsubst %,$(COREDIR)/ui/%,$(_COREUIS))

_MODELINCS = ConfigurationRepository.h SensorInputProxy.h KeyListener.h ModelInitializerProxy.h ModelUi.h Log.h Recorder.h 
MODELINCS = $(patsubst %,$(COREDIR)/include/%,$(_MODELINCS))

_MODELINITS = IModelInitializer.h ModelAnticipateInitializer.h ModelInitializer.h ModelLayerInitializer.h ModelLifeInitializer.h ModelNeuronInitializer.h ModelSonataInitializer.h ParticleModelInitializer.h 
MODELINITS = $(patsubst %,$(COREDIR)/include/Initializers/%,$(_MODELINITS))

_MODELSENSORS = ISensorInput.h SensorInputFile.h SensorSonataFile.h 
MODELSENSORS = $(patsubst %,$(COREDIR)/include/SensorInputs/%,$(_MODELSENSORS))


all: $(COREINCS) $(CORECONFIGS) $(COREUIS) $(MODELINCS) $(MODELINITS) $(MODELSENSORS) 
.PHONY: all

# Individual include files
$(COREDIR)/include/NeuronRecordCommon.h: spiking-model/include/NeuronRecordCommon.h
	cp spiking-model/include/NeuronRecordCommon.h $(COREDIR)/include/

# All files in the config folder
$(COREDIR)/config/a1.json: spiking-model/config/a1.json
	cp spiking-model/config/a1.json $(COREDIR)/config/

$(COREDIR)/config/bmtk1.json: spiking-model/config/bmtk1.json
	cp spiking-model/config/bmtk1.json $(COREDIR)/config/

$(COREDIR)/config/l1.json: spiking-model/config/l1.json
	cp spiking-model/config/l1.json $(COREDIR)/config/

$(COREDIR)/config/Anticipate/Anticipate.json: spiking-model/config/Anticipate/Anticipate.json
	cp spiking-model/config/Anticipate/Anticipate.json $(COREDIR)/config/Anticipate/

$(COREDIR)/config/Anticipate/I1-I2-N1-N2.json: spiking-model/config/Anticipate/I1-I2-N1-N2.json
	cp spiking-model/config/Anticipate/I1-I2-N1-N2.json $(COREDIR)/config/Anticipate/

$(COREDIR)/config/BMTK1/bmtk.json: spiking-model/config/BMTK1/bmtk.json
	cp spiking-model/config/BMTK1/bmtk.json $(COREDIR)/config/BMTK1/

$(COREDIR)/config/BMTK1/2-5.json: spiking-model/config/BMTK1/2-5.json
	cp spiking-model/config/BMTK1/2-5.json $(COREDIR)/config/BMTK1/

$(COREDIR)/config/Layer/Layer.json: spiking-model/config/Layer/Layer.json
	cp spiking-model/config/Layer/Layer.json $(COREDIR)/config/Layer/

$(COREDIR)/config/Layer/Layer1-5.json: spiking-model/config/Layer/Layer1-5.json
	cp spiking-model/config/Layer/Layer1-5.json $(COREDIR)/config/Layer/

# All files in the ui folder
$(COREDIR)/ui/Makefile: spiking-model/ui/Makefile
	cp spiking-model/ui/Makefile $(COREDIR)/ui/

$(COREDIR)/ui/mec.py: spiking-model/ui/mec.py
	cp spiking-model/ui/mec.py $(COREDIR)/ui/

$(COREDIR)/ui/mem.py: spiking-model/ui/mem.py
	cp spiking-model/ui/mem.py $(COREDIR)/ui/

$(COREDIR)/ui/mes.py: spiking-model/ui/mes.py
	cp spiking-model/ui/mes.py $(COREDIR)/ui/

$(COREDIR)/ui/mev.py: spiking-model/ui/mev.py
	cp spiking-model/ui/mev.py $(COREDIR)/ui/

$(COREDIR)/ui/nnclean.py: spiking-model/ui/nnclean.py
	cp spiking-model/ui/nnclean.py $(COREDIR)/ui/

$(COREDIR)/ui/nngenanticipate.py: spiking-model/ui/nngenanticipate.py
	cp spiking-model/ui/nngenanticipate.py $(COREDIR)/ui/

$(COREDIR)/ui/nngenlayer.py: spiking-model/ui/nngenlayer.py
	cp spiking-model/ui/nngenlayer.py $(COREDIR)/ui/

$(COREDIR)/ui/nnplot.py: spiking-model/ui/nnplot.py
	cp spiking-model/ui/nnplot.py $(COREDIR)/ui/

$(COREDIR)/ui/nn.py: spiking-model/ui/nn.py
	cp spiking-model/ui/nn.py $(COREDIR)/ui/

$(COREDIR)/ui/nntidy.py: spiking-model/ui/nntidy.py
	cp spiking-model/ui/nntidy.py $(COREDIR)/ui/

$(COREDIR)/ui/np.py: spiking-model/ui/np.py
	cp spiking-model/ui/np.py $(COREDIR)/ui/


### Reverse the path of files obtained through modelengine/add-dependencies

# Some individual files from the include folder
$(COREDIR)/include/ConfigurationRepository.h: include/ConfigurationRepository.h
	cp include/ConfigurationRepository.h $(COREDIR)/include/

$(COREDIR)/include/SensorInputProxy.h: include/sdk/SensorInputProxy.h
	cp include/sdk/SensorInputProxy.h $(COREDIR)/include/

$(COREDIR)/include/KeyListener.h: include/sdk/KeyListener.h
	cp include/sdk/KeyListener.h $(COREDIR)/include/

$(COREDIR)/include/ModelInitializerProxy.h: include/sdk/ModelInitializerProxy.h
	cp include/sdk/ModelInitializerProxy.h $(COREDIR)/include/

$(COREDIR)/include/ModelUi.h: include/sdk/ModelUi.h
	cp include/sdk/ModelUi.h $(COREDIR)/include/

$(COREDIR)/include/Log.h: include/Log.h
	cp include/Log.h $(COREDIR)/include/

$(COREDIR)/include/Recorder.h: include/Recorder.h
	cp include/Recorder.h $(COREDIR)/include/

# All files from the include/Initializers folder
$(COREDIR)/include/Initializers/IModelInitializer.h: include/Initializers/IModelInitializer.h
	cp s/Initializers/IModelInitializer.h $(COREDIR)/include/Initializers/

$(COREDIR)/include/Initializers/ModelAnticipateInitializer.h: include/Initializers/ModelAnticipateInitializer.h
	cp include/Initializers/ModelAnticipateInitializer.h $(COREDIR)/include/Initializers/

$(COREDIR)/include/Initializers/ModelInitializer.h: include/Initializers/ModelInitializer.h
	cp include/Initializers/ModelInitializer.h $(COREDIR)/include/Initializers/

$(COREDIR)/include/Initializers/ModelLayerInitializer.h: include/Initializers/ModelLayerInitializer.h
	cp include/Initializers/ModelLayerInitializer.h $(COREDIR)/include/Initializers/

$(COREDIR)/include/Initializers/ModelLifeInitializer.h: include/Initializers/ModelLifeInitializer.h
	cp include/Initializers/ModelLifeInitializer.h $(COREDIR)/include/Initializers/

$(COREDIR)/include/Initializers/ModelNeuronInitializer.h: include/Initializers/ModelNeuronInitializer.h
	cp include/Initializers/ModelNeuronInitializer.h $(COREDIR)/include/Initializers/

$(COREDIR)/include/Initializers/ModelSonataInitializer.h: include/Initializers/ModelSonataInitializer.h
	cp include/Initializers/ModelSonataInitializer.h $(COREDIR)/include/Initializers/

$(COREDIR)/include/Initializers/ParticleModelInitializer.h: include/Initializers/ParticleModelInitializer.h
	cp include/Initializers/ParticleModelInitializer.h $(COREDIR)/include/Initializers/

# All files from the include/SensorInputs folder
$(COREDIR)/include/SensorInputs/ISensorInput.h: include/SensorInputs/ISensorInput.h
	cp include/SensorInputs/ISensorInput.h $(COREDIR)/include/SensorInputs/

$(COREDIR)/include/SensorInputs/SensorInputFile.h: include/SensorInputs/SensorInputFile.h
	cp include/SensorInputs/SensorInputFile.h $(COREDIR)/include/SensorInputs/

$(COREDIR)/include/SensorInputs/SensorSonataFile.h: include/SensorInputs/SensorSonataFile.h
	cp include/SensorInputs/SensorSonataFile.h $(COREDIR)/include/SensorInputs/
