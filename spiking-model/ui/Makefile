BDIR=../bin

_INPUTGENDEPS = nngenlayer.py nngenanticipate.py
INPUTGENDEPS = $(patsubst %,$(BDIR)/%,$(_INPUTGENDEPS))

_FRAMEWORKDEPS = nn.py nnclean.py nnplot.py nntidy.py mes.py 
FRAMEWORKDEPS = $(patsubst %,$(BDIR)/%,$(_FRAMEWORKDEPS))

_DEPRECATEDDEPS = mem.py mev.py mec.py np.py
DEPRECATEDDEPS = $(patsubst %,$(BDIR)/%,$(_DEPRECATEDDEPS))

all: $(INPUTGENDEPS) $(FRAMEWORKDEPS)
.PHONY: all

$(BDIR)/mes.py: mes.py
	cp mes.py $(BDIR)/

$(BDIR)/mem.py: mem.py
	cp mem.py $(BDIR)/

$(BDIR)/mev.py: mev.py
	cp mev.py $(BDIR)/

$(BDIR)/mec.py: mec.py
	cp mec.py $(BDIR)/

$(BDIR)/np.py: np.py
	cp np.py $(BDIR)/

$(BDIR)/nn.py: nn.py
	cp nn.py $(BDIR)/

$(BDIR)/nnclean.py: nnclean.py
	cp nnclean.py $(BDIR)/

$(BDIR)/nnplot.py: nnplot.py
	cp nnplot.py $(BDIR)/

$(BDIR)/nntidy.py: nntidy.py
	cp nntidy.py $(BDIR)/

$(BDIR)/nngenlayer.py: nngenlayer.py
	cp nngenlayer.py $(BDIR)/

$(BDIR)/nngenanticipate.py: nngenanticipate.py
	cp nngenanticipate.py $(BDIR)/

