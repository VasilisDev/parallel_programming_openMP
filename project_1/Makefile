OMPCPP    = g++
OMPC      = gcc


OPENMP   = -fopenmp
CFLAGS   = -Wall  $(OPENMP)

TARGETS  = diagonal multisort

program: $(TARGETS)

diagonal: diagonal.cpp
	$(OMPCPP) $< $(CFLAGS) -o $@

multisort: multisort.c
	$(OMPC) $< $(CFLAGS) -o $@


clean:
	rm -f $(TARGETS)
