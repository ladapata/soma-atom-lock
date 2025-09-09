#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

long int soma = 0;
pthread_mutex_t mutex;

pthread_cond_t cond; /* variavel de cond para bloqueio condicional*/

short int is_finished = 1;

void* ExecutaTarefa (void* args) {
  long int id = (long int) args;
  printf("Thread : %ld esta executando...\n", id);

  for (int i=0; i<10000; i++) {
    pthread_mutex_lock(&mutex);

    if(is_finished)
    {
      if(!(soma%1000) && (soma!=0)) pthread_cond_wait(&cond, &mutex); /* condicao de bloqueio */
    }

    soma++;
    pthread_mutex_unlock(&mutex);
  }
  printf("Thread : %ld terminou!\n", id);
  pthread_exit(NULL);
}

void* extra (void* args) {
  long int temp = 1;
  printf("Extra : esta executando...\n");
  for (int i=0; i<1000000; i++) {

    pthread_mutex_lock(&mutex);
    if (!(soma % 1000)){
      
/*apenas para evitar de imprimir multiplos iguais*/
if(!(temp^soma)){pthread_cond_broadcast(&cond); pthread_mutex_unlock(&mutex); continue;}
temp = soma;
/*apenas para evitar de imprimir multiplos iguais*/

      printf("soma = %ld \n", temp);

      pthread_cond_broadcast(&cond); /* condicao foi atingida (soma foi impresso) */

    }
    pthread_mutex_unlock(&mutex);
  }

/* apenas para garantir que o programa sempre termine de executar quando a thread EXTRA termina antes das outras */
  is_finished = 0;
  pthread_cond_broadcast(&cond);
/* apenas para garantir que o programa sempre termine de executar quando a thread EXTRA termina antes das outras */

  printf("Extra : terminou!\n");
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  pthread_t *tid;
  int nthreads;

  if(argc<2) {
    printf("Digite: %s <numero de threads>\n", argv[0]);
    return 1;
  }
  nthreads = atoi(argv[1]);

  tid = (pthread_t*) malloc(sizeof(pthread_t)*(nthreads+1));
  if(tid==NULL) {puts("ERRO--malloc"); return 2;}

  pthread_mutex_init(&mutex, NULL);

  /* inicializa cond para travar as threads sob uma condicao especifica*/
  pthread_cond_init(&cond, NULL);

  for(long int t=0; t<nthreads; t++) {
    if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }

  if (pthread_create(&tid[nthreads], NULL, extra, NULL)) {
    printf("--ERRO: pthread_create()\n"); exit(-1);
  }

  for (int t=0; t<nthreads+1; t++) {
    if (pthread_join(tid[t], NULL)) {
      printf("--ERRO: pthread_join() \n"); exit(-1); 
    } 
  } 

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);
   
  printf("Valor de 'soma' = %ld\n", soma);

  return 0;
}