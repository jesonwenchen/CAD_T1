# T1 MPI — Resultados dos Testes

## Ambiente de Teste
- **SO**: WSL Ubuntu 24.04.1 LTS (via Windows)
- **MPI**: OpenMPI 4.1.6
- **Compilador**: `mpicc -O2 -Wall`

## Resumo

| Exercício | Programa | Compila | Executa | Resultado |
|-----------|----------|---------|---------|-----------|
| Ex1(a) | `hello_os` | ✅ | ✅ | PIDs distintos, CPU reportado |
| Ex1(b) | `hello_ring` | ✅ | ✅ | Anel sem deadlock, todas saudações coletadas |
| Ex2 | `mpi_trap_generalized` | ✅ | ✅ | p=3,n=10 → 1.9835 ≈ 2.0 (erro 1.65e-02) |
| Ex3 | `mpi_psum` | ✅ | ✅ | Erro relativo = 1.62e-15 (FP reordering) |
| Ex4(a) | `hello_gather` | ✅ | ✅ | Saudações na ordem dos ranks |
| Ex4(b) | `minmax` | ✅ | ✅ | Min/Max idênticos ao serial |
| Ex5 | `mpi_vecadd_gather` | ✅ | ✅ | 1000 elementos corretos |
| Ex5 | `mpi_vecadd_allgather` | ✅ | ✅ | Todos ranks com z completo |
| Ex6 | `student_struct` | ✅ | ✅ | 1 Bcast, dados corretos |
| Ex6 | `student_three_bcasts` | ✅ | ✅ | 3 Bcasts, dados idênticos |

## Testes de Borda (p=1)

| Programa | p=1 | Resultado |
|----------|-----|-----------|
| `mpi_trap_generalized` | ✅ | Sem divisão por zero, erro 1.93e-13 |
| `mpi_psum` | ✅ | Erro = 0 (mesma ordem de soma) |
| `minmax` | ✅ | Correto |

---

## Saídas Detalhadas

### Ex1(a) — hello_os (p=8)
```
Hello from rank 0/8 -- PID = 13, CPU = 0
Hello from rank 1/8 -- PID = 14, CPU = 0
Hello from rank 5/8 -- PID = 23, CPU = 0
Hello from rank 2/8 -- PID = 15, CPU = 0
Hello from rank 3/8 -- PID = 16, CPU = 0
Hello from rank 4/8 -- PID = 20, CPU = 0
Hello from rank 6/8 -- PID = 26, CPU = 0
Hello from rank 7/8 -- PID = 29, CPU = 0
```

> [!NOTE]
> Todos os PIDs são distintos. CPU=0 para todos porque o WSL pode virtualizar um único core. Em hardware real, os cores variam entre execuções.

### Ex1(b) — hello_ring (p=8)
```
=== Saudações coletadas pelo rank 0 (via anel) ===
  [rank 0]: Hello from rank 0/8
  [rank 1]: Hello from rank 1/8
  [rank 2]: Hello from rank 2/8
  ...
  [rank 7]: Hello from rank 7/8
Rank 0 recebeu do vizinho à esquerda (rank 7): Hello from rank 7/8
```
✅ Sem deadlock, anel funcionou corretamente.

### Ex2(a) — Trapézio (p=3, n=10)
```
Parâmetros: a=0.000000, b=3.141593, n=10, p=3
Distribuição de trapézios: 1 ranks com 4, 2 ranks com 3
Integral aproximada = 1.983523537509454
Valor exato          = 2.000000000000000
Erro absoluto        = 1.65e-02
```
✅ Distribuição 4+3+3 = 10 correta.

### Ex3 — Soma paralela (p=4, N=10000)
```
Soma serial   = 503587.138643203827087
Soma paralela = 503587.138643203012180
Erro absoluto = 8.15e-10
Erro relativo = 1.62e-15
Resultado: CORRETO (erro devido a reordenação de FP)
```

### Ex4(b) — Min/Max (p=4, N=1000)
```
Mínimo global (paralelo) = -499.268294311719160
Mínimo global (serial)   = -499.268294311719160
Máximo global (paralelo) = 498.879869467988556
Máximo global (serial)   = 498.879869467988556
Verificação: CORRETO
```

### Ex5 — Vecadd (p=4, N=1000)
```
Verificação: CORRETO (todos os 1000 elementos conferem)
z[0..4]     = 0.0 3.0 6.0 9.0 12.0
z[N-5..N-1] = 2985.0 2988.0 2991.0 2994.0 2997.0
```

### Ex6 — Tipo derivado vs. três Bcasts
```
[Tipo derivado] Total de chamadas MPI_Bcast: 1
[Três Bcasts]   Total de chamadas MPI_Bcast: 3
Conteúdo recebido: IDÊNTICO em ambas as versões
```

## Correção Aplicada

- `mpi_trap_generalized.c`: alterado formato de `T_p` de `%.6f` para `%.6e` (notação científica) para mostrar tempos sub-milissegundo corretamente.
