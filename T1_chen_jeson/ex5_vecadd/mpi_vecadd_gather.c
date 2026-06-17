/* mpi_vecadd_gather.c — Exercício 5 */
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

    /* Alocação dos vetores globais (apenas rank 0) */
    double *x = NULL, *y = NULL, *z = NULL;
    if (my_rank == 0) {
        x = malloc((size_t)N * sizeof(double));
        y = malloc((size_t)N * sizeof(double));
        z = malloc((size_t)N * sizeof(double));
        if (!x || !y || !z) {
            fprintf(stderr, "Erro de alocação\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        /* Inicializar: x_i = i, y_i = 2*i */
        for (int i = 0; i < N; i++) {
            x[i] = (double)i;
            y[i] = 2.0 * (double)i;
        }
    }

    /* Alocação dos vetores locais */
    double *local_x = malloc((size_t)local_n * sizeof(double));
    double *local_y = malloc((size_t)local_n * sizeof(double));
    double *local_z = malloc((size_t)local_n * sizeof(double));

    /* Distribuir com Scatter */
    MPI_Scatter(x, local_n, MPI_DOUBLE,
                local_x, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(y, local_n, MPI_DOUBLE,
                local_y, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    /* Cálculo local: z = x + y */
    for (int i = 0; i < local_n; i++) {
        local_z[i] = local_x[i] + local_y[i];
    }

    /* Coletar z no processo 0 com Gather */
    MPI_Gather(local_z, local_n, MPI_DOUBLE,
               z, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    /* Verificação serial */
    if (my_rank == 0) {
        int errors = 0;
        for (int i = 0; i < N; i++) {
            double expected = (double)i + 2.0 * (double)i; /* 3*i */
            if (fabs(z[i] - expected) > 1e-10) {
                errors++;
                if (errors <= 5) {
                    printf("ERRO em z[%d]: obtido=%.2f, esperado=%.2f\n",
                           i, z[i], expected);
                }
            }
        }

        printf("N = %d, p = %d\n", N, comm_sz);
        if (errors == 0) {
            printf("Verificação: CORRETO (todos os %d elementos conferem)\n", N);
        } else {
            printf("Verificação: %d erros encontrados\n", errors);
        }

        /* Mostrar primeiros e últimos elementos */
        printf("z[0..4]     = ");
        for (int i = 0; i < 5 && i < N; i++) printf("%.1f ", z[i]);
        printf("\n");
        printf("z[N-5..N-1] = ");
        for (int i = N - 5; i < N; i++) printf("%.1f ", z[i]);
        printf("\n");

        free(x); free(y); free(z);
    }

    free(local_x); free(local_y); free(local_z);
    MPI_Finalize();
    return 0;
}
