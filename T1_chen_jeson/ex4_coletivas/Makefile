CC      = mpicc
CFLAGS  = -O2 -Wall
LDFLAGS = -lm
P       ?= 4

PROGS = hello_gather minmax

all: $(PROGS)

hello_gather: hello_gather.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

minmax: minmax.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

run: all
	@echo "=== hello_gather ==="
	mpiexec -n $(P) ./hello_gather
	@echo ""
	@echo "=== minmax ==="
	mpiexec -n $(P) ./minmax

clean:
	rm -f $(PROGS) *.o
