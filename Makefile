install: ./include/ModelEngine.h
.PHONY: install

./include/ModelEngine.h:
	./add-dependencies
	./dockb

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~
