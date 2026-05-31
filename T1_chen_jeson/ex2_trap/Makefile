CC      = mpicc
CFLAGS  = -O2 -Wall
LDFLAGS = -lm
P       ?= 4
PI      = 3.14159265358979323846

PROG = mpi_trap_generalized

all: $(PROG)

$(PROG): $(PROG).c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Execução padrão: p=4, n=10000, sin(x) em [0, pi]
run: all
	mpiexec -n $(P) ./$(PROG) 0 $(PI) 10000

# Verificação do exercício 2(a): p=3, n=10
run-verify: all
	mpiexec -n 3 ./$(PROG) 0 $(PI) 10

# Estudo de escalabilidade forte: n=10^7, p in {1,2,4,8}
run-scaling: all
	@echo "=== Escalabilidade Forte (n=10000000) ==="
	@for p in 1 2 4 8; do \
		echo "--- p=$$p ---"; \
		mpiexec -n $$p ./$(PROG) 0 $(PI) 10000000; \
		echo ""; \
	done

# Bônus: convergência O(h²), p=4
run-convergence: all
	@echo "=== Estudo de Convergência (p=4) ==="
	@for n in 64 256 1024 4096 16384; do \
		echo "--- n=$$n ---"; \
		mpiexec -n 4 ./$(PROG) 0 $(PI) $$n; \
		echo ""; \
	done

clean:
	rm -f $(PROG) *.o
