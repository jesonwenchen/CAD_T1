/*
 * mpi_trap_generalized.c — Exercício 2
 * Regra do trapézio generalizada para n arbitrário (p não precisa
 * dividir n). Inclui:
 *   (a) Balanceamento de carga para n arbitrário
 *   (b) Estudo de escalabilidade forte com MPI_Wtime
 *   (c) Bônus: verificação de convergência O(h²)
 *
 * Compilar: mpicc -O2 -Wall -o mpi_trap_generalized mpi_trap_generalized.c -lm
 * Executar: mpiexec -n <p> ./mpi_trap_generalized <a> <b> <n>
 *
 * Exemplo de verificação: mpiexec -n 3 ./mpi_trap_generalized 0 3.14159265358979 10
 *   Resultado esperado ≈ 2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

/* Função a integrar: f(x) = sin(x) */
double f(double x) {
    return sin(x);
}

/* Regra do trapézio local */
double trap(double local_a, double local_b, int local_n, double h) {
    double estimate = (f(local_a) + f(local_b)) / 2.0;
    for (int i = 1; i < local_n; i++) {
        estimate += f(local_a + i * h);
    }
    estimate *= h;
    return estimate;
}

int main(int argc, char *argv[]) {
    int my_rank, comm_sz;
    double a, b;
    int n;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    /* Leitura dos parâmetros */
    if (argc != 4) {
        if (my_rank == 0) {
            fprintf(stderr, "Uso: mpiexec -n <p> %s <a> <b> <n>\n", argv[0]);
            fprintf(stderr, "Exemplo: mpiexec -n 3 %s 0 3.14159265358979 10\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }
    a = atof(argv[1]);
    b = atof(argv[2]);
    n = atoi(argv[3]);

    double h = (b - a) / n;

    /*
     * (a) Balanceamento de carga para n arbitrário:
     * Os primeiros (n % p) ranks recebem ceil(n/p) trapézios,
     * os demais recebem floor(n/p).
     */
    int rem     = n % comm_sz;
    int local_n = n / comm_sz + (my_rank < rem ? 1 : 0);
    int offset  = my_rank * (n / comm_sz) + (my_rank < rem ? my_rank : rem);
    double local_a = a + offset * h;
    double local_b = local_a + local_n * h;

    /* (b) Cronometragem para escalabilidade forte */
    MPI_Barrier(MPI_COMM_WORLD);
    double t_start = MPI_Wtime();

    double local_integral = trap(local_a, local_b, local_n, h);

    double total_integral = 0.0;
    MPI_Reduce(&local_integral, &total_integral, 1, MPI_DOUBLE,
               MPI_SUM, 0, MPI_COMM_WORLD);

    double t_end = MPI_Wtime();
    double local_elapsed = t_end - t_start;

    /* Encontrar o tempo máximo entre todos os processos */
    double max_elapsed = 0.0;
    MPI_Reduce(&local_elapsed, &max_elapsed, 1, MPI_DOUBLE,
               MPI_MAX, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        double exact = 2.0; /* integral de sin(x) de 0 a pi */
        double error = fabs(total_integral - exact);

        printf("Parâmetros: a=%.6f, b=%.6f, n=%d, p=%d\n", a, b, n, comm_sz);
        printf("Distribuição de trapézios: %d ranks com %d, %d ranks com %d\n",
               rem, n / comm_sz + 1, comm_sz - rem, n / comm_sz);
        printf("Integral aproximada = %.15f\n", total_integral);
        printf("Valor exato          = %.15f\n", exact);
        printf("Erro absoluto        = %.2e\n", error);
        printf("h                    = %.2e\n", h);
        printf("T_p (tempo paralelo) = %.6e s\n", max_elapsed);
    }

    MPI_Finalize();
    return 0;
}
