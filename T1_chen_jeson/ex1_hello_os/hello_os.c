#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
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
