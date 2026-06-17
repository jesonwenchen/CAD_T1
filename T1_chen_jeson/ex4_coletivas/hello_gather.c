/* hello_gather.c — Exercício 4(a) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MAX_MSG 256

int main(int argc, char *argv[]) {
    int my_rank, comm_sz;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    /* Cada rank prepara sua saudação */
    char local_msg[MAX_MSG];
    snprintf(local_msg, MAX_MSG,
             "Hello from rank %d/%d", my_rank, comm_sz);

    /* Buffer para receber todas as saudações (só usado no rank 0) */
    char *all_msgs = NULL;
    if (my_rank == 0) {
        all_msgs = malloc((size_t)comm_sz * MAX_MSG);
        if (!all_msgs) {
            fprintf(stderr, "Erro de alocação\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    /* Gather: cada rank envia MAX_MSG chars, rank 0 recebe tudo */
    MPI_Gather(local_msg, MAX_MSG, MPI_CHAR,
               all_msgs,  MAX_MSG, MPI_CHAR,
               0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        printf("=== Saudações coletadas via MPI_Gather ===\n");
        for (int i = 0; i < comm_sz; i++) {
            printf("  [rank %d]: %s\n", i, all_msgs + i * MAX_MSG);
        }
        free(all_msgs);
    }

    MPI_Finalize();
    return 0;
}
