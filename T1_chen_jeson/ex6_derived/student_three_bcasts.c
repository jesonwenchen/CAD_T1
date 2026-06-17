/* student_three_bcasts.c — Exercício 6 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

struct Student {
    char   name[50];
    double grade;
    int    id;
};

int main(int argc, char *argv[]) {
    int my_rank, comm_sz;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    struct Student s;

    if (my_rank == 0) {
        /* Processo 0 preenche o registro */
        strncpy(s.name, "Jeson Chen", 50);
        s.name[49] = '\0';
        s.grade = 9.5;
        s.id = 12345;
        printf("Rank 0: enviando Student { name=\"%s\", grade=%.1f, id=%d }\n",
               s.name, s.grade, s.id);
    }

    /* Três chamadas MPI_Bcast separadas, uma por campo */
    MPI_Bcast(s.name,  50, MPI_CHAR,   0, MPI_COMM_WORLD);  /* 1ª */
    MPI_Bcast(&s.grade, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);  /* 2ª */
    MPI_Bcast(&s.id,    1, MPI_INT,    0, MPI_COMM_WORLD);   /* 3ª */

    if (my_rank != 0) {
        printf("Rank %d: recebeu Student { name=\"%s\", grade=%.1f, id=%d }\n",
               my_rank, s.name, s.grade, s.id);
    }

    /* Número de chamadas MPI_Bcast: 3 */
    if (my_rank == 0) {
        printf("\n[Três Bcasts] Total de chamadas MPI_Bcast: 3\n");
        printf("\nComparação:\n");
        printf("  - Versão com tipo derivado: 1 chamada MPI_Bcast\n");
        printf("  - Versão com três Bcasts:   3 chamadas MPI_Bcast\n");
        printf("  - O conteúdo recebido é IDÊNTICO em ambas as versões.\n");
        printf("  - O tipo derivado é mais eficiente: reduz o número de\n");
        printf("    mensagens (1 vs 3), diminuindo a latência total,\n");
        printf("    especialmente relevante em redes de alta latência.\n");
    }

    MPI_Finalize();
    return 0;
}
