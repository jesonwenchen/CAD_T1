CC      = mpicc
CFLAGS  = -O2 -Wall
LDFLAGS = -lm
P       ?= 4

PROGS = hello_os hello_ring

all: $(PROGS)

hello_os: hello_os.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

hello_ring: hello_ring.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

run: all
	@echo "=== hello_os ==="
	mpiexec -n $(P) ./hello_os
	@echo ""
	@echo "=== hello_ring ==="
	mpiexec -n $(P) ./hello_ring

clean:
	rm -f $(PROGS) *.o
