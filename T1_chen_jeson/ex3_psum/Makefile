CC      = mpicc
CFLAGS  = -O2 -Wall
LDFLAGS = -lm
P       ?= 4
N       ?= 10000

PROG = mpi_psum

all: $(PROG)

$(PROG): $(PROG).c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

run: all
	mpiexec -n $(P) ./$(PROG) $(N)

clean:
	rm -f $(PROG) *.o
