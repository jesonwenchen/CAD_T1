CC      = mpicc
CFLAGS  = -O2 -Wall
LDFLAGS = -lm
P       ?= 4
N       ?= 1000

PROGS = mpi_vecadd_gather mpi_vecadd_allgather

all: $(PROGS)

mpi_vecadd_gather: mpi_vecadd_gather.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

mpi_vecadd_allgather: mpi_vecadd_allgather.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

run: all
	@echo "=== Versão Gather ==="
	mpiexec -n $(P) ./mpi_vecadd_gather $(N)
	@echo ""
	@echo "=== Versão Allgather ==="
	mpiexec -n $(P) ./mpi_vecadd_allgather $(N)

clean:
	rm -f $(PROGS) *.o
