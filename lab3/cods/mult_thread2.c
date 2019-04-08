#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"  //para usar funcoes de medida de tempo
#include <unistd.h> //para usar a funcao "sysconf"

void* multiplicaMatrizVetor(void* tid);
double computa (int nthreads);
void imprimeMatriz(float *mat, int linhas, int colunas, FILE *arq);
int preencheMatriz(float **mat, int linhas, int colunas, FILE *arq);
int preencheVetor(float **vet, int dim, FILE *arq);
void imprimeVetor(float *vet, int dim, FILE *arq);
double criaThread(int _nthreads);

//variaveis globais
float *matA; //matriz de entrada
float *vetX; //vetor de entrada
float *vetB; //vetor de saida
int linhas, colunas, dim; //variaves aux de matrizes
int nthreads; //variaveis de threads
double inicio, fim; //variaveis de marcação tempo
double delta1, delta2, delta3, delta4; //variaveis de tempo


//funcao que multiplica matriz por vetor (A * X = B)
//entrada: matriz de entrada, vetor de entrada, vetor de saida, dimensoes da matriz
//requisito 1: o numero de colunas da matriz eh igual ao numero de elementos do vetor de entrada
//requisito 2: o numero de linhas da matriz eh igual ao numero de elementos do vetor de saida
void* multiplicaMatrizVetor(void* tid)
{
    int i = * (int *) tid; int j;
    int inicio, fim; //intervalo de elementos processados por cada thread
    //printf("mult nthreds: %d\n", nthreads);
    int tam_bloco = dim/nthreads;
    //printf("dim: %d, tam bloco: %d\n", dim, tam_bloco);
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
    printf("nthreads: %d\n", nthreads);
    if (nthreads > dim) nthreads = dim;
    //printf("n. de threads: %d\n(o limite de threads e %d. se voce escolheu um valor maior, o numero de threads sera igual a %d.\n", nthreads, dim, dim);
    
    double tn[nthreads];
    double speedup[nthreads];
    double baux = 0;
    
    criaThread(nthreads);
    
    /*
    for (int aux=1; aux<=nthreads; aux++) 
    {
        tn[aux-1] = criaThread(aux);
        tn[aux-1] = criaThread(aux);
        v_baux[aux-1] = (tn[aux-1]-(tn[0]/nthreads))/(tn[0]-(tn[0]/nthreads)); baux += v_baux[aux-1];
        printf("t[1] = %.3lf    t[%d] = %.3lf\n", tn[0], aux, tn[aux-1]);
        printf("baux para thread %d: %.8lf\n", aux, v_baux[aux-1]);
    }
    */
    
    /*
    for (int aux=1; aux<=nthreads; aux++)
    {
        speedup[aux-1] = tn[0]/tn[aux-1];
        printf("speedup de 1 para %d threads: %.5lf\n", aux, speedup[aux-1]);
    }
    */
    
    /*
    baux = baux/nthreads;
    printf("baux final: %.8lf\n", baux);
    criaThread(nthreads);
    */
    
    //libera os espacos de memoria alocados
    free(matA); free(vetX); free(vetB);
            
    GET_TIME(fim)
    delta1 = fim - inicio; //delta1 - tempo de recebimento de entradas
    //printf("tempo para %d loops: %.8lf\n", nthreads, delta1);
    
    return 0;
}

double criaThread(int _nthreads)
{
    GET_TIME(inicio);
    pthread_t *tid_sistema; //vetor de identificadores das threads no sistema
    int *tid;
    
    //aloca espaco para o vetor de identificadores das threads no sistema
    tid_sistema = (pthread_t *) malloc(sizeof(pthread_t) * _nthreads);
    if(tid_sistema==NULL) { printf("--ERRO: malloc()\n"); exit(-1); }
            
    //cria as threads
    for(int t=0; t<_nthreads; t++)
    {            
        tid = malloc(sizeof(int));
        if(tid==NULL) { printf("--ERRO: malloc()\n"); exit(-1); }
        *tid = t;
        //printf("tid %d\n", tid);
        //printf("tid_sistema: %d\n", tid_sistema);
        
        if (pthread_create(&tid_sistema[t], NULL, multiplicaMatrizVetor, (void*) tid))
        { printf("--ERRO: pthread_create()\n"); exit(-1); }
        //printf("execucao %d da thread %d\n", t+1, *tid_sistema);
                
    }
            
    //espera todas as threads terminarem
    for(int t=0; t<_nthreads; t++)
    {
        if (pthread_join(tid_sistema[t], NULL)) { printf("--ERRO: pthread_join()\n"); exit(-1); }
    }
    
    GET_TIME(fim)
    delta2 = fim - inicio; //delta1 - tempo de recebimento de entradas
    //printf("tempo para %d threads: %.8lf\n", _nthreads, delta2);
    
    //libera os espacos de memoria alocados
    free(tid_sistema);
    return delta2;
}
