/* mpi_vecadd_allgather.c — Exercício 5 (versão Allgather) */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int my_rank, comm_sz;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    int N = 1000;
    if (argc > 1) {
        N = atoi(argv[1]);
    }

    /* Ajustar N para ser divisível por p */
    if (N % comm_sz != 0) {
        N = N + (comm_sz - N % comm_sz);
        if (my_rank == 0)
            printf("N ajustado para %d (divisível por %d)\n", N, comm_sz);
    }

    int local_n = N / comm_sz;

    /* Vetores globais (apenas rank 0 inicializa x e y) */
    double *x = NULL, *y = NULL;
    if (my_rank == 0) {
        x = malloc((size_t)N * sizeof(double));
        y = malloc((size_t)N * sizeof(double));
        if (!x || !y) {
            fprintf(stderr, "Erro de alocação\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        for (int i = 0; i < N; i++) {
            x[i] = (double)i;
            y[i] = 2.0 * (double)i;
        }
    }

    /* Vetores locais */
    double *local_x = malloc((size_t)local_n * sizeof(double));
    double *local_y = malloc((size_t)local_n * sizeof(double));
    double *local_z = malloc((size_t)local_n * sizeof(double));

    /* z global — MPI_Allgather distribui a todos */
    double *z = malloc((size_t)N * sizeof(double));

    /* Distribuir com Scatter */
    MPI_Scatter(x, local_n, MPI_DOUBLE,
                local_x, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(y, local_n, MPI_DOUBLE,
                local_y, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    /* Cálculo local: z = x + y */
    for (int i = 0; i < local_n; i++) {
        local_z[i] = local_x[i] + local_y[i];
    }

    /* Allgather: todos os ranks recebem z completo */
    MPI_Allgather(local_z, local_n, MPI_DOUBLE,
                  z, local_n, MPI_DOUBLE, MPI_COMM_WORLD);

    /* Cada rank pode verificar (demonstramos com rank 0 e último rank) */
    if (my_rank == 0 || my_rank == comm_sz - 1) {
        int errors = 0;
        for (int i = 0; i < N; i++) {
            double expected = 3.0 * (double)i;
            if (fabs(z[i] - expected) > 1e-10) {
                errors++;
            }
        }
        printf("Rank %d: verificação do z completo — %s (%d erros)\n",
               my_rank, errors == 0 ? "CORRETO" : "FALHA", errors);
    }

    if (my_rank == 0) {
        printf("\nN = %d, p = %d\n", N, comm_sz);
        printf("Todos os ranks possuem o vetor z completo (via MPI_Allgather).\n");
        printf("z[0..4]     = ");
        for (int i = 0; i < 5 && i < N; i++) printf("%.1f ", z[i]);
        printf("\n");
        printf("z[N-5..N-1] = ");
        for (int i = N - 5; i < N; i++) printf("%.1f ", z[i]);
        printf("\n");

        free(x); free(y);
    }

    free(local_x); free(local_y); free(local_z); free(z);
    MPI_Finalize();
    return 0;
}
