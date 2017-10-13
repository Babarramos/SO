
/*--------------------------------------------------------------------
| Disclamer: Este código foi feito um pouco à pressa. Não
|            está perfeito mas ilustra a utilização da mplib
| v2: alguns warnings corrigidos.
---------------------------------------------------------------------*/

/*--------------------------------------------------------------------
| Includes
---------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#include "mplib3.h"

/*--------------------------------------------------------------------
| Define
---------------------------------------------------------------------*/

#define STRSZ 10
#define BUFFSZ 256

typedef struct {

  int iteracoes;
  int id;
  double** matrix;
  int idx;

} argsSimular_t;

/*--------------------------------------------------------------------
| Function: strupr
| Description: converte uma string para uppercase
---------------------------------------------------------------------*/

/*char *strupr (char *str){
  int i, n;

  n = strlen (str);
//  for (i=0; i<n; i++)
  //  str[i]= toupper(str[i]);
  return str;
}*/

/*--------------------------------------------------------------------
| Function: slaveThread
| Description: envia numa mensagem para a thread 0 e espera uma resposta
|              faz isto n vezes
---------------------------------------------------------------------*/

void *slaveThread(void *a) {

   char send_buff[BUFFSZ];
   char receive_buff[BUFFSZ];
   argsSimular_t *thread   = (argsSimular_t *) a;
   int myid = thread->id;
   int i,j;

   for (i = 0; i < STRSZ; i++)
     send_buff[i] = 96+myid;
   send_buff[10] = 0;

   for (j = 0; j < thread->iteracoes; j++) {

     printf ("task=%d vai enviar %s para task 0\n", myid, send_buff);
     enviarMensagem (myid, 0, send_buff, strlen(send_buff)+1);


     receberMensagem (0, myid, receive_buff, BUFFSZ);
     printf ("task=%d recebeu %s da task 0\n", myid, receive_buff);
   }
   return 0;
}

/*--------------------------------------------------------------------
| Function:
| Description: cria n threads
|              para cada um dos filhos recebe uma mensagem e envia
               uma resposta n vezes
---------------------------------------------------------------------*/
argsSimular_t* createThreads(int numTarefas, int it, int k, DoubleMatrix2D *matrix) {

  argsSimular_t* slaves = (argsSimular_t*)malloc(numTarefas*sizeof(argsSimular_t));
  /* create slaves */
  int h = 0;
  for (int i = 0; i < numTarefas; i++) {

    slaves[i].matrix = malloc(sizeof(double*) * k);//matrix[h:h + k];
    for(int a = 0; a < k; a++)
      slaves[i].matrix[a] = malloc(k * numTarefas * sizeof(double));

    int line = 0;
    for(int a = h; a < h + k; a++) {
      for(int col = 0; col < numTarefas * k; col++)
        slaves[i].matrix[line][col] = dm2dGetEntry(matrix, a, col);
      line++;
    }

    h += k;

    slaves[i].idx = i;
    slaves[i].iteracoes = it;
    slaves[i].id = pthread_create(&slaves[i], NULL, slaveThread, &slaves[i]);

  }
  return slaves;

}


/*int main (int argc, char** argv) {
  int numTarefas;
  int i, j;
  char buff[BUFFSZ];

  pthread_t *slaves;
  printf("PNI\n" );
  if (argc < 2) {
    printf("client_server_1 n_tarefas\n");
    return 1;
  }

  numTarefas = atoi(argv[1]);

  if ((numTarefas<1) || (numTarefas>26)){
    printf ("n_tarefas deve ser entre 1 e 26\n");
    return 1;
  }


  if (inicializarMPlib(1, numTarefas + 1) == -1) {

    printf("Erro ao inicializar MPLib.\n");
    return 1;
  }

  for (i=0; i<numTarefas; i++) {
    for (j=0; j<numTarefas; j++) {
      receberMensagem (i+1, 0, buff, BUFFSZ);
      strupr (buff);
      enviarMensagem (0, i+1, buff, strlen(buff)+1);
    }
  }

  /* Esperar que os Escravos Terminem */


  void killThreads(int numTarefas, argsSimular_t *slave_args) {
    int i;
    for (i = 0; i < numTarefas; i++) {
      if (pthread_join(slave_args[i].id, NULL)) {

        free(slave_args[i].matrix);
        fprintf(stderr, "\nErro ao esperar por um escravo.\n");
        return -1;
      }
  }
  return 0;
}
