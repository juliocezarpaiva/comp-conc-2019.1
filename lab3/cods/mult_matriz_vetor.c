    /* Disciplina: Computacao Concorrente */
    /* Prof.: Silvana Rossetto */
    /* Laboratório: 2 */
    /* Codigo: Multiplica uma matriz por um vetor */

    #include <stdio.h>
    #include <stdlib.h>
    #include <pthread.h>
    #include "timer.h"  //para usar funcoes de medida de tempo
    #include <unistd.h> //para usar a funcao "sysconf"

    //variaveis globais
    float *matA; //matriz de entrada
    float *vetX; //vetor de entrada
    float *vetB; //vetor de saida
    int nthreads;
    int linhas, colunas; //dimensoes da matriz de entrada
    int dim; //dimensao do vetor de entrada
    int *tid;
    double inicio, fim, delta1, delta2, delta3, delta4; //variaveis para medir o tempo de execucao

    //funcao que multiplica matriz por vetor (A * X = B)
    //entrada: matriz de entrada, vetor de entrada, vetor de saida, dimensoes da matriz
    //requisito 1: o numero de colunas da matriz eh igual ao numero de elementos do vetor de entrada
    //requisito 2: o numero de linhas da matriz eh igual ao numero de elementos do vetor de saida
    void* multiplicaMatrizVetor(void* tid)
    {
        int i = * (int *) tid; int j;
        int inicio, fim; //intervalo de elementos processados por cada thread
        int tam_bloco = dim/nthreads;
        inicio = i * tam_bloco;
        //o ultimo fluxo trata os elementos restantes
        if (i<nthreads-1) fim = inicio + tam_bloco; 
        else fim = dim;
        for(i=inicio; i<fim; i++)
        {
            vetB[i] = 0;
            for (j=0; j<colunas; j++)
            {
                vetB[i] += matA[i*colunas+j] * vetX[j];
            }
        }
        free(tid);
        pthread_exit(NULL);
    }

    //funcao que aloca espaco para uma matriz e preenche seus valores
    //entrada: matriz de entrada, dimensoes da matriz
    //saida: retorna 1 se a matriz foi preenchida com sucesso e 0 caso contrario
    int preencheMatriz(float **mat, int linhas, int colunas, FILE *arq)
    {
        GET_TIME(inicio);
        
        int i, j;
        //aloca espaco de memoria para a matriz
        *mat = (float*) malloc(sizeof(float) * linhas * colunas);
        if (mat == NULL) return 0;
        //preenche o vetor
        for (i=0; i<linhas; i++)
        {
            for (j=0; j<colunas; j++)
            {
                //fscanf(arq, "%f", *( (*mat) + (i*colunas+j) ) );
                fscanf(arq, "%f", (*mat) + (i*colunas+j));
            }
        }
        
        GET_TIME(fim);
        delta3 += fim - inicio;
        
        return 1;
    }

    //funcao que imprime uma matriz
    //entrada: matriz de entrada, dimensoes da matriz
    //saida: matriz impressa na tela
    void imprimeMatriz(float *mat, int linhas, int colunas, FILE *arq)
    {
        int i, j;
        for (i=0; i<linhas; i++)
        {
            for (j=0; j<colunas; j++)
            {
                fprintf(arq, "%.1f ", mat[i*colunas+j]);
            }
            fprintf(arq, "\n");
        }
    }

    //funcao que aloca espaco para um vetor e preenche seus valores
    //entrada: vetor de entrada, dimensao do vetor
    //saida: retorna 1 se o vetor foi preenchido com sucesso e 0 caso contrario
    int preencheVetor(float **vet, int dim, FILE *arq)
    {
    int i;
        //aloca espaco de memoria para o vetor
        *vet = (float*) malloc(sizeof(float) * dim);
        if (vet == NULL) return 0;
        //preenche o vetor
        for (i=0; i<dim; i++)
        {
            //*( (*vet)+i ) = 1.0;
            fscanf(arq, "%f", (*vet) + i);
        }
        return 1;
    }

    //funcao que imprime um vetor
    //entrada: vetor de entrada, dimensao do vetor
    //saida: vetor impresso na tela
    void imprimeVetor(float *vet, int dim, FILE *arq)
    {
        int i;
        for (i=0; i<dim; i++)
        {
            fprintf(arq, "%.1f ", vet[i]);
        }
        fprintf(arq, "\n");
    }

    //funcao principal
    int main(int argc, char *argv[])
    {
        FILE *arqA, *arqX, *arqB; //arquivos dos dados de entrada e saida
        pthread_t *tid_sistema; //vetor de identificadores das threads no sistema

        //---------------------------------------------------------------------------------------------
        
        GET_TIME(inicio);
        
        //le e valida os parametros de entrada
        //o arquivo da matriz de entrada deve conter na primeira linha as dimensoes da matriz (linha coluna) seguido dos elementos da matriz separados por espaco
        //o arquivo do vetor de entrada deve conter na primeira linha a dimensao do vetor seguido dos elementos separados por espaco
        if(argc < 5)
        {
            fprintf(stderr, "Digite: %s <arquivo matriz A> <arquivo vetor X> <arquivo vetor B> <num de Threads desejadas>.\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        
        //abre o arquivo da matriz de entrada
        arqA = fopen(argv[1], "r");
        if(arqA == NULL) { fprintf(stderr, "Erro ao abrir o arquivo da matriz de entrada.\n"); exit(EXIT_FAILURE); }
        
        //le as dimensoes da matriz de entrada
        fscanf(arqA, "%d", &linhas);
        fscanf(arqA, "%d", &colunas);

        //abre o arquivo do vetor de entrada
        arqX = fopen(argv[2], "r");
        if(arqX == NULL) { fprintf(stderr, "Erro ao abrir o arquivo do vetor de entrada.\n"); exit(EXIT_FAILURE); }
        
        //le a dimensao do vetor de entrada
        fscanf(arqX, "%d", &dim);

        //valida as dimensoes da matriz e vetor de entrada
        if(colunas != dim) { fprintf(stderr, "Erro: as dimensoes da matriz e do vetor de entrada nao sao compativeis.\n"); exit(EXIT_FAILURE); }

        //abre o arquivo do vetor de saida
        arqB = fopen(argv[3], "w");
        if(arqB == NULL) { fprintf(stderr, "Erro ao abrir o arquivo do vetor de saida.\n"); exit(EXIT_FAILURE); }

        //aloca e preenche a matriz de entrada
        if(preencheMatriz(&matA, linhas, colunas, arqA) == 0) {
            fprintf(stderr, "Erro de preenchimento da matriz de entrada\n");
            exit(EXIT_FAILURE);
        }
        //aloca e preenche o vetor de entrada
        if(preencheVetor(&vetX, dim, arqX) == 0) { fprintf(stderr, "Erro de preenchimento do vetor de entrada\n"); exit(EXIT_FAILURE); }
        
        //aloca o vetor de saida
        vetB = (float*) malloc(sizeof(float) * linhas);
        if(vetB==NULL) { fprintf(stderr, "Erro de alocacao do vetor de saida\n"); exit(EXIT_FAILURE); }
        
        //recebe num de threads
        nthreads = atoi(argv[4]);
        if (nthreads > dim) nthreads = dim;
        printf("n. de threads: %d\n(o limite de threads e %d. se voce escolheu um valor maior, o numero de threads sera igual a %d.\n", nthreads, dim, dim);

        GET_TIME(fim)
        delta1 = fim - inicio; //delta1 - tempo de recebimento de entradas
        
        //---------------------------------------------------------------------------------------------
        
        GET_TIME(inicio);
        
        //aloca espaco para o vetor de identificadores das threads no sistema
        tid_sistema = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
        if(tid_sistema==NULL) { printf("--ERRO: malloc()\n"); exit(-1); }
        
        //cria as threads
        for(int t=0; t<nthreads; t++)
        {            
            tid = malloc(sizeof(int)); if(tid==NULL) { printf("--ERRO: malloc()\n"); exit(-1); }
            *tid = t;

            if (pthread_create(&tid_sistema[t], NULL, multiplicaMatrizVetor, (void*) tid)) { printf("--ERRO: pthread_create()\n"); exit(-1); }
            
        }
        
        //espera todas as threads terminarem
        for(int t=0; t<nthreads; t++)
        {
            if (pthread_join(tid_sistema[t], NULL)) { printf("--ERRO: pthread_join()\n"); exit(-1); }
        }
        
        GET_TIME(fim);
        delta2 = fim - inicio; //delta2 - tempo de criação e execuçao das threads
        
        //---------------------------------------------------------------------------------------------
        
        GET_TIME(inicio);
        
        //imprime o vetor de saida no arquivo de saida
        imprimeVetor(vetB, linhas, arqB);

        GET_TIME(fim);
        delta3 = fim - inicio; //delta3 - tempo de impressao
        
        //---------------------------------------------------------------------------------------------
        
        GET_TIME(inicio);
        
        //libera os espacos de memoria alocados
        free(matA); free(vetX); free(vetB);
        free(tid_sistema);
        
        GET_TIME(fim);
        delta4 = fim - inicio; //delta4 - tempo de finalizaçao
        
        //---------------------------------------------------------------------------------------------
        
        //exibe os tempos gastos em cada parte do programa 
        printf("Tempo inicializacoes: %.8lf\n", delta1);
        printf("Tempo mult com %d threads e %d tam do vet resultante: %.8lf\n", nthreads, dim, delta2);
        printf("Tempo finalizacoes: %.8lf\n", delta3);
        printf("Tempo finalizacoes: %.8lf\n", delta4);
        
        return 0;
    }

