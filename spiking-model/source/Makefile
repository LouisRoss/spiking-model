DEBUGFLAG=

all:
	make --directory=NeuronModel DEBUGFLAG="$(DEBUGFLAG)"
	make --directory=Initializers DEBUGFLAG="$(DEBUGFLAG)"
	make --directory=SensorInputs DEBUGFLAG="$(DEBUGFLAG)"
.PHONY: all

debug:
	make DEBUGFLAG="-g"
.PHONY: debug

