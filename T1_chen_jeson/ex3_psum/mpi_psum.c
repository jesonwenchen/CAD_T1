/*
 * mpi_psum.c — Exercício 3
 * Soma paralela usando comunicação ponto-a-ponto.
 *
 * - Processo 0 gera um vetor de N doubles aleatórios.
 * - Processo 0 distribui N/p elementos a cada processo via MPI_Send.
 * - Cada processo calcula sua soma local.
 * - Cada processo envia soma parcial ao processo 0.
 * - Processo 0 reduz por adição e imprime o total.
 * - Compara com soma serial e reporta erro relativo.
 *
 * Nota: assume p | N para simplificar (N é ajustado se necessário).
 *
 * Compilar: mpicc -O2 -Wall -o mpi_psum mpi_psum.c -lm
 * Executar: mpiexec -n <p> ./mpi_psum [N]
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int my_rank, comm_sz;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    int N = 10000;
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
    if (!local_vec) {
        fprintf(stderr, "Rank %d: erro ao alocar memória\n", my_rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    double serial_sum = 0.0;
    double *global_vec = NULL;

    if (my_rank == 0) {
        /* Gerar vetor aleatório */
        global_vec = malloc((size_t)N * sizeof(double));
        if (!global_vec) {
            fprintf(stderr, "Rank 0: erro ao alocar memória\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        srand(42);
        for (int i = 0; i < N; i++) {
            global_vec[i] = (double)rand() / RAND_MAX * 100.0;
        }

        /* Soma serial para verificação */
        for (int i = 0; i < N; i++) {
            serial_sum += global_vec[i];
        }

        /* Copiar bloco local do rank 0 */
        for (int i = 0; i < local_n; i++) {
            local_vec[i] = global_vec[i];
        }

        /* Distribuir blocos aos demais ranks via MPI_Send */
        for (int dest = 1; dest < comm_sz; dest++) {
            MPI_Send(global_vec + dest * local_n, local_n, MPI_DOUBLE,
                     dest, 0, MPI_COMM_WORLD);
        }
    } else {
        /* Receber bloco do rank 0 */
        MPI_Recv(local_vec, local_n, MPI_DOUBLE,
                 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    /* Soma local */
    double local_sum = 0.0;
    for (int i = 0; i < local_n; i++) {
        local_sum += local_vec[i];
    }

    /* Enviar/receber somas parciais */
    double parallel_sum = 0.0;
    if (my_rank != 0) {
        MPI_Send(&local_sum, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
    } else {
        parallel_sum = local_sum;
        for (int src = 1; src < comm_sz; src++) {
            double partial;
            MPI_Recv(&partial, 1, MPI_DOUBLE, src, 1,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            parallel_sum += partial;
        }
    }

    if (my_rank == 0) {
        double abs_error = fabs(parallel_sum - serial_sum);
        double rel_error = (serial_sum != 0.0) ?
                           abs_error / fabs(serial_sum) : abs_error;

        printf("N = %d, p = %d\n", N, comm_sz);
        printf("Soma serial   = %.15f\n", serial_sum);
        printf("Soma paralela = %.15f\n", parallel_sum);
        printf("Erro absoluto = %.2e\n", abs_error);
        printf("Erro relativo = %.2e\n", rel_error);

        if (rel_error < 1e-10) {
            printf("Resultado: CORRETO (erro devido a reordenação de FP)\n");
        } else {
            printf("Resultado: DIVERGENTE (verifique o código)\n");
        }

        free(global_vec);
    }

    free(local_vec);
    MPI_Finalize();
    return 0;
}
