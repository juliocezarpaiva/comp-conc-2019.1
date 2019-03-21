/* Disciplina: Computacao Concorrente */
/* Prof.: Daniel S. */
/* Laboratório: 1 */
/* Codigo: Incrementa de 1 cada elemento de um vetor de N (10 < N < 100) elementos */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define NTHREADS 2

typedef struct
{
    int tam, id, *vec;
} t_Arg;

//funcao executada pelas threads
void *Incrementa (void *arg)
{
    //printf("o meu deus\n");
    t_Arg *_arg = (t_Arg *) arg;
    int thread_parte = _arg->id;
    int n = _arg->tam;
    //-----------------------------------------------------------
    if(n%2 == 1) n++;
    for ( int i = thread_parte*(n/2); i < (thread_parte+1)*(n/2); i++ ) { _arg->vec[i]++; }
    //-----------------------------------------------------------
    free(arg);
    pthread_exit(NULL);
}

//funcao principal do programa 
int main(void)
{
    pthread_t tid_sistema[NTHREADS]; //identificadoes das threads no sistema
    int t, n, *v; //variavel auxiliar
    t_Arg *arg;
    //-----------------------------------------------------------
    printf("Ql. tamanho N do vetor? (10 < N < 100)");
    scanf("%d", &n);
    //n = 5;
    //-----------------------------------------------------------
    v = malloc(sizeof(int)*n);
    if (v == NULL) {
        printf("--ERRO: malloc(v)\n"); exit(-1);
    }
    for(int i = 0; i < n; i++) { scanf("%d", &v[i]); printf("acabou de entrar: %d\n", v[i]); } // Entra com os valores iniciais do vetor
    //-----------------------------------------------------------
    for(t=0; t<NTHREADS; t++) {
        printf("--Aloca e preenche argumentos para thread %d\n", t);
        arg = malloc(sizeof(t_Arg));
        if (arg == NULL) {
            printf("--ERRO: malloc(arg)\n"); exit(-1);
        }
        arg->tam = n;
        arg->id = t;
        arg->vec= v;
        //-----------------------------------------------------------
        printf("--Cria a thread %d\n", t);
        if (pthread_create(&tid_sistema[t], NULL, Incrementa, (void*) arg)) {
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }
    //--espera todas as threads terminarem
    for (t=0; t<NTHREADS; t++) {
        if (pthread_join(tid_sistema[t], NULL)) {
            printf("--ERRO: pthread_join() \n"); exit(-1); 
        }
    }
    //-----------------------------------------------------------
    printf("vetor modificado:\n");
    for( int i = 0; i < n; i++ ) printf("%d\n", v[i]);
    printf("--Thread principal terminou\n");
    //-----------------------------------------------------------
    free(v);
    pthread_exit(NULL);
}
