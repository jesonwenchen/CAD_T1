/*
 * hello_ring.c — Exercício 1(b)
 * Variante em anel: processo k envia saudação ao processo (k+1) % p
 * e recebe do processo (k-1+p) % p.
 * O processo 0 coleta e imprime todas as saudações.
 *
 * Usa MPI_Sendrecv para evitar deadlock: cada rank envia e recebe
 * simultaneamente, eliminando a dependência circular que ocorreria
 * se fizéssemos Recv-antes-de-Send.
 *
 * Compilar: mpicc -O2 -Wall -o hello_ring hello_ring.c -lm
 * Executar: mpiexec -n 8 ./hello_ring
 */
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
    /* O +comm_sz garante argumento não-negativo para o módulo,
       pois em C o operador % preserva o sinal do dividendo. */

    /* Preparar saudação local */
    char send_buf[MAX_MSG];
    char recv_buf[MAX_MSG];
    snprintf(send_buf, MAX_MSG, "Hello from rank %d/%d", my_rank, comm_sz);

    /*
     * Estratégia anti-deadlock: MPI_Sendrecv.
     * Cada rank envia sua saudação ao vizinho à direita e recebe do
     * vizinho à esquerda em uma única chamada atômica. Isso evita o
     * deadlock que ocorreria se todos fizessem Recv antes de Send
     * (todos esperariam eternamente pelo vizinho à esquerda).
     *
     * Alternativa válida: reordenar Send/Recv para ranks pares/ímpares.
     * Escolhemos MPI_Sendrecv por ser mais simples e funcionar para
     * qualquer número de processos sem tratamento de casos especiais.
     */
    MPI_Sendrecv(send_buf, MAX_MSG, MPI_CHAR, dest, 0,
                 recv_buf, MAX_MSG, MPI_CHAR, src,  0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    if (my_rank == 0) {
        /* Processo 0: imprime a saudação recebida do último rank e
         * coleta as saudações que passaram pelo anel dos demais ranks. */
        char *all_msgs = malloc((size_t)comm_sz * MAX_MSG);
        if (!all_msgs) { MPI_Abort(MPI_COMM_WORLD, 1); }

        /* Copiar a mensagem que rank 0 recebeu (vinda de rank p-1) */
        memcpy(all_msgs + 0 * MAX_MSG, recv_buf, MAX_MSG);

        /* Agora receber as mensagens que cada rank k (k>=1) recebeu
         * do seu vizinho à esquerda e repassou adiante.
         * Para manter a ordem dos ranks originais, o rank k envia
         * ao rank 0 a saudação que ele recebeu (ou seja, a saudação
         * do rank k-1). Mas nós queremos imprimir por rank original.
         * Simplificação: cada rank (exceto 0) envia recv_buf ao rank 0.
         * recv_buf do rank k contém a saudação do rank k-1.
         * Portanto all_msgs[k] = saudação de rank k-1. */

        /* Na verdade, para imprimir na ordem dos ranks, vamos fazer
         * diferente: cada rank envia sua PRÓPRIA saudação (send_buf)
         * ao rank 0, e o rank 0 as imprime em ordem. */
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
