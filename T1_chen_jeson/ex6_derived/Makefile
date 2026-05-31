CC      = mpicc
CFLAGS  = -O2 -Wall
LDFLAGS = -lm
P       ?= 4

PROGS = student_struct student_three_bcasts

all: $(PROGS)

student_struct: student_struct.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

student_three_bcasts: student_three_bcasts.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

run: all
	@echo "=== Versão com tipo derivado (1 Bcast) ==="
	mpiexec -n $(P) ./student_struct
	@echo ""
	@echo "=== Versão com três Bcasts ==="
	mpiexec -n $(P) ./student_three_bcasts

clean:
	rm -f $(PROGS) *.o
