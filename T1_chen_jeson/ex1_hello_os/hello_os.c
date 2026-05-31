/*
 * hello_os.c — Exercício 1(a)
 * Cada processo MPI reporta seu rank, tamanho do comunicador,
 * PID do SO e núcleo de CPU em que está rodando.
 *
 * Compilar: mpicc -O2 -Wall -o hello_os hello_os.c -lm
 * Executar: mpiexec -n 8 ./hello_os
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    /* getpid() */
#include <sched.h>     /* sched_getcpu() */
#include <mpi.h>

int main(int argc, char *argv[]) {
    int my_rank, comm_sz;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    int pid = (int)getpid();
    int cpu = sched_getcpu();

    printf("Hello from rank %d/%d -- PID = %d, CPU = %d\n",
           my_rank, comm_sz, pid, cpu);

    MPI_Finalize();
    return 0;
}
