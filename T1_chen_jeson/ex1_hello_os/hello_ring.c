/* hello_ring.c — Exercício 1(b) */
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

    /* Vizinhos no anel */
    int dest = (my_rank + 1) % comm_sz;
    int src  = (my_rank - 1 + comm_sz) % comm_sz;
    /* +comm_sz garante argumento não-negativo para o módulo */

    /* Preparar saudação local */
    char send_buf[MAX_MSG];
    char recv_buf[MAX_MSG];
    snprintf(send_buf, MAX_MSG, "Hello from rank %d/%d", my_rank, comm_sz);

    /* Estratégia anti-deadlock: MPI_Sendrecv */
    MPI_Sendrecv(send_buf, MAX_MSG, MPI_CHAR, dest, 0,
                 recv_buf, MAX_MSG, MPI_CHAR, src,  0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    if (my_rank == 0) {
        /* Processo 0: coleta saudações e imprime */
        char *all_msgs = malloc((size_t)comm_sz * MAX_MSG);
        if (!all_msgs) { MPI_Abort(MPI_COMM_WORLD, 1); }

        /* Copiar a mensagem que rank 0 recebeu (vinda de rank p-1) */
        memcpy(all_msgs + 0 * MAX_MSG, recv_buf, MAX_MSG);

        /* Copia recv_buf da etapa anterior e coleta os demais */

        /* Cada rank envia sua própria saudação ao rank 0 */
        memcpy(all_msgs + 0 * MAX_MSG, send_buf, MAX_MSG);
        for (int k = 1; k < comm_sz; k++) {
            MPI_Recv(all_msgs + k * MAX_MSG, MAX_MSG, MPI_CHAR,
                     k, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        printf("=== Saudações coletadas pelo rank 0 (via anel) ===\n");
        for (int k = 0; k < comm_sz; k++) {
            printf("  [rank %d]: %s\n", k, all_msgs + k * MAX_MSG);
        }
        /* Também mostrar que o anel funcionou */
        printf("\nRank 0 recebeu do vizinho à esquerda (rank %d): %s\n",
               src, recv_buf);

        free(all_msgs);
    } else {
        /* Ranks != 0 enviam sua própria saudação ao rank 0 */
        MPI_Send(send_buf, MAX_MSG, MPI_CHAR, 0, 1, MPI_COMM_WORLD);

        /* Mostrar que o anel funcionou para cada rank */
        printf("Rank %d recebeu do vizinho à esquerda (rank %d): %s\n",
               my_rank, src, recv_buf);
    }

    MPI_Finalize();
    return 0;
}
