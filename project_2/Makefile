OMPC      = gcc


OPENMP   = -fopenmp
CFLAGS   = -Wall  $(OPENMP)

TARGETS  = multisort

program: $(TARGET)


multisort: multisort.c
	$(OMPC) $< $(CFLAGS) -o $@


clean:
	rm -f $(TARGET)
