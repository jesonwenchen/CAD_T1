/* minmax.c — Exercício 4(b) */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
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
    double *local_vec = malloc((size_t)local_n * sizeof(double));
    double *global_vec = NULL;

    if (my_rank == 0) {
        global_vec = malloc((size_t)N * sizeof(double));
        if (!global_vec) {
            fprintf(stderr, "Erro de alocação\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        /* Gerar vetor */
        srand(42);
        for (int i = 0; i < N; i++) {
            global_vec[i] = (double)rand() / RAND_MAX * 1000.0 - 500.0;
        }
    }

    /* Distribuir com Scatter */
    MPI_Scatter(global_vec, local_n, MPI_DOUBLE,
                local_vec, local_n, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    /* Calcular mín e máx locais */
    double local_min = local_vec[0];
    double local_max = local_vec[0];
    for (int i = 1; i < local_n; i++) {
        if (local_vec[i] < local_min) local_min = local_vec[i];
        if (local_vec[i] > local_max) local_max = local_vec[i];
    }

    /* Reduzir para obter mín e máx globais */
    double global_min, global_max;
    MPI_Reduce(&local_min, &global_min, 1, MPI_DOUBLE,
               MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_max, &global_max, 1, MPI_DOUBLE,
               MPI_MAX, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        /* Verificação serial */
        double serial_min = global_vec[0];
        double serial_max = global_vec[0];
        for (int i = 1; i < N; i++) {
            if (global_vec[i] < serial_min) serial_min = global_vec[i];
            if (global_vec[i] > serial_max) serial_max = global_vec[i];
        }

        printf("N = %d, p = %d\n", N, comm_sz);
        printf("Mínimo global (paralelo) = %.15f\n", global_min);
        printf("Mínimo global (serial)   = %.15f\n", serial_min);
        printf("Máximo global (paralelo) = %.15f\n", global_max);
        printf("Máximo global (serial)   = %.15f\n", serial_max);

        int ok = (fabs(global_min - serial_min) < DBL_EPSILON * 100.0) &&
                 (fabs(global_max - serial_max) < DBL_EPSILON * 100.0);
        printf("Verificação: %s\n", ok ? "CORRETO" : "FALHA");

        free(global_vec);
    }

    free(local_vec);
    MPI_Finalize();
    return 0;
}
