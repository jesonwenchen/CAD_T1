# Programação Paralela com MPI — Guia de Estudo

Este documento explica os conceitos teóricos abordados no trabalho e detalha como cada um dos códigos implementados funciona. Ele foi escrito para ajudar você a entender a matéria e se preparar para apresentações ou provas.

---

## 1. O que é MPI?

**MPI** (*Message Passing Interface*) é um padrão para programação paralela em sistemas de memória distribuída (como clusters de computadores, mas também funciona muito bem nos múltiplos núcleos de um único PC). 

### Paradigma SPMD
Em MPI, usamos o paradigma **SPMD** (*Single Program, Multiple Data*). Isso significa que **o mesmo executável** é rodado simultaneamente por vários processos. Cada processo é chamado de **Rank** (identificado por um número de `0` até `p-1`, onde `p` é o número total de processos).

Como todos rodam o mesmo código, nós usamos `if (my_rank == 0)` para fazer um processo agir como o "mestre" ou "coordenador", enquanto os outros podem assumir o papel de "trabalhadores". Eles não compartilham memória (variáveis de um não afetam os de outro); toda troca de informação precisa ser feita enviando e recebendo mensagens através da rede ou barramento interno.

---

## 2. Explicação dos Códigos

### Exercício 1(a): `hello_os.c` — Introspecção do SO
Neste exercício, exploramos o fato de que cada Rank MPI é, de fato, um processo real do Sistema Operacional.
- **O que faz:** Cada rank pede ao SO o seu Process ID (`getpid()`) e pergunta ao escalonador em qual núcleo (CPU core) está rodando no momento (`sched_getcpu()`).
- **Aprendizado:** Quando você roda `mpiexec -n 8`, o SO cria 8 processos independentes. Eles podem rodar simultaneamente em núcleos diferentes, e o próprio kernel do Linux (CFS) decide dinamicamente qual processo vai para qual núcleo.

### Exercício 1(b): `hello_ring.c` — Topologia em Anel
Aqui fazemos os processos passarem uma mensagem em um formato de "anel": o rank 0 manda pro 1, que manda pro 2, ..., que manda pro último, que manda de volta pro 0.
- **A Matemática:** O vizinho da direita (destino) é `(my_rank + 1) % p`. O da esquerda (origem) é `(my_rank - 1 + p) % p`. O `+ p` garante que não teremos números negativos antes de aplicar o módulo.
- **O Problema do Deadlock:** Se todos os processos chamarem `MPI_Recv` primeiro, todos ficarão bloqueados esperando mensagens, e o programa trava para sempre (Deadlock).
- **A Solução:** Usamos `MPI_Sendrecv`. Essa função executa o envio e o recebimento simultaneamente, resolvendo a dependência circular e garantindo que o programa não trave.

### Exercício 2: `mpi_trap_generalized.c` — Regra do Trapézio Generalizada
Este código calcula a integral de uma função (área sob a curva de $\sin(x)$) dividindo a curva em $n$ trapézios. Em paralelo, dividimos esses $n$ trapézios entre os $p$ processos.
- **Balanceamento de Carga:** E se $n = 10$ e $p = 3$? A divisão não é exata. O código calcula `rem = n % p` (o resto da divisão, que dá 1). Os primeiros `rem` processos ganham 1 trapézio a mais. Então o Rank 0 calcula 4 trapézios, e os Ranks 1 e 2 calculam 3 cada. 
- **Escalabilidade (Speedup e Eficiência):** Usamos `MPI_Wtime()` para medir quanto tempo leva.
  - **$S_p$ (Speedup)** = Tempo com 1 proc / Tempo com $p$ procs. Mede quantas vezes mais rápido ficou.
  - **$E_p$ (Eficiência)** = $S_p / p$. Mede o quão bem estamos aproveitando os núcleos. O ideal é perto de 1.0 (ou 100%). Se $E_p$ cai muito para um $p$ grande, significa que o tempo gasto comunicando dados superou o tempo fazendo conta.

### Exercício 3: `mpi_psum.c` — Soma Paralela (Ponto-a-Ponto)
Dividimos um grande vetor de números para que vários processos somem pedaços dele simultaneamente.
- **Como funciona:** O Rank 0 tem o vetor inteiro. Ele envia (`MPI_Send`) um bloco para o Rank 1, outro para o Rank 2, etc. Cada um calcula sua sub-soma e envia o sub-total de volta ao Rank 0 (`MPI_Send`), que recebe as partes (`MPI_Recv`) e soma tudo.
- **Problema Numérico:** A soma feita em paralelo vai dar um erro minúsculo em comparação com a soma feita sequencialmente. Por que? Porque as contas de ponto flutuante (`double`) sofrem arredondamentos na CPU. Como na paralela estamos somando partes em uma ordem diferente da sequencial, os arredondamentos acumulam de forma ligeiramente diferente (quebra da associatividade).

### Exercício 4: `hello_gather.c` e `minmax.c` — Comunicação Coletiva
Ao invés de fazer um laço de repetição (`for`) com vários `MPI_Send` e `MPI_Recv`, o MPI oferece as **Coletivas**. Elas envolvem todos os processos de uma vez e são altamente otimizadas na rede.
- **Gather:** Em `hello_gather.c`, cada processo gera sua mensagem e, com uma única chamada de `MPI_Gather`, o Rank 0 recebe todas e já as empacota organizadas em um array.
- **Scatter + Reduce:** Em `minmax.c`, o Rank 0 "picota" o vetor e distribui com `MPI_Scatter`. Cada um acha o Mínimo e Máximo local. Por fim, usamos `MPI_Reduce(..., MPI_MAX, ...)`: ele pega o máximo de cada processo, compara todos, e entrega o valor máximo absoluto (global) ao Rank 0.

### Exercício 5: `mpi_vecadd_gather.c` e `mpi_vecadd_allgather.c`
Soma de vetores: $Z = X + Y$. O vetor X e o Y são "Scatterizados" entre os processos. Cada processo soma seu pedaço local. 
- **Gather vs Allgather:** 
  - Com **Gather**, só o Rank 0 termina sabendo o resultado final de $Z$. Isso é útil se só formos imprimir o resultado ou salvar num arquivo.
  - Com **Allgather**, ao final da operação, **todos os Ranks** terão uma cópia do vetor $Z$ completo em suas memórias. Isso é vital se $Z$ for necessário para a próxima etapa do algoritmo de todos os trabalhadores.

### Exercício 6: Tipos Derivados (Structs) em `student_struct.c`
O MPI sabe enviar dados primários (INT, DOUBLE, CHAR). Mas e se tivermos uma `struct` misturando texto e números?
- **O Modo Ruim (Três Bcasts):** Transmitir o nome (`MPI_Bcast(MPI_CHAR)`), depois a nota (`MPI_DOUBLE`), depois o ID (`MPI_INT`). Isso exige disparar 3 mensagens pela rede. A **latência** (tempo só para o pacote sair e chegar) vai punir a performance do sistema três vezes.
- **O Modo Bom (Tipo Derivado):** Criamos um tipo novo (uma "receita" MPI) mapeando quantos bytes cada campo tem. Usamos `offsetof` para lidar com o "padding" (espaço vazio que o compilador às vezes coloca entre os campos na memória para alinhar os dados). Assim, enviamos a struct inteira com **apenas 1 chamada** de `MPI_Bcast`. Menos idas à rede = maior performance.
