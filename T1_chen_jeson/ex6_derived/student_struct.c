/*
 * student_struct.c — Exercício 6 (versão com tipo derivado)
 * Define um tipo derivado MPI para struct Student e transmite o
 * registro com UMA única chamada MPI_Bcast.
 *
 * Usa offsetof() de <stddef.h> para calcular os deslocamentos
 * corretamente, respeitando o padding do compilador.
 *
 * Compilar: mpicc -O2 -Wall -o student_struct student_struct.c -lm
 * Executar: mpiexec -n <p> ./student_struct
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
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

    /* Construir tipo derivado MPI para struct Student */
    struct Student s;

    /* 3 blocos: name (50 chars), grade (1 double), id (1 int) */
    int          block_lengths[3] = {50, 1, 1};
    MPI_Aint     displacements[3];
    MPI_Datatype types[3] = {MPI_CHAR, MPI_DOUBLE, MPI_INT};

    /* Usar offsetof para obter deslocamentos corretos */
    displacements[0] = offsetof(struct Student, name);
    displacements[1] = offsetof(struct Student, grade);
    displacements[2] = offsetof(struct Student, id);

    MPI_Datatype mpi_student_type;
    MPI_Type_create_struct(3, block_lengths, displacements, types,
                           &mpi_student_type);
    MPI_Type_commit(&mpi_student_type);

    if (my_rank == 0) {
        /* Processo 0 preenche o registro */
        strncpy(s.name, "Jeson Chen", 50);
        s.name[49] = '\0';
        s.grade = 9.5;
        s.id = 12345;
        printf("Rank 0: enviando Student { name=\"%s\", grade=%.1f, id=%d }\n",
               s.name, s.grade, s.id);
    }

    /* UMA única chamada MPI_Bcast com o tipo derivado */
    MPI_Bcast(&s, 1, mpi_student_type, 0, MPI_COMM_WORLD);

    if (my_rank != 0) {
        printf("Rank %d: recebeu Student { name=\"%s\", grade=%.1f, id=%d }\n",
               my_rank, s.name, s.grade, s.id);
    }

    /* Número de chamadas MPI_Bcast: 1 */
    if (my_rank == 0) {
        printf("\n[Tipo derivado] Total de chamadas MPI_Bcast: 1\n");
    }

    MPI_Type_free(&mpi_student_type);
    MPI_Finalize();
    return 0;
}
