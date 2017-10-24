
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

double** simul(argsSimular_t *thread) {

  double **m, **aux, **tmp;
  int iter, i, j;
  double value;

  m = thread->matrix;
  int linhas = sizeof(m)/sizeof(double);
  int colunas = sizeof(m[0])/sizeof(double);

  if(linhas < 2 || colunas < 2)
    return NULL;

  int numIteracoes = thread->iteracoes;

  for (iter = 0; iter < numIteracoes; iter++) {

    for (i = 1; i < linhas - 1; i++)
      for (j = 1; j < colunas - 1; j++) {
        value = ( m[i - 1][j] + m[i + 1][j] + m[i][j - 1] + m[i][j + 1] ) / 4.0;
          aux[i][j] = value;
      }

    tmp = aux;
    aux = m;
    m = tmp;
  }

  return m;
}

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
    if(myid % 2 == 0) {

      receberMensagem (0, myid, receive_buff, BUFFSZ);
      printf ("task=%d recebeu %s da task 0\n", myid, receive_buff);

      printf ("task=%d vai enviar %s para task 0\n", myid, send_buff);
      enviarMensagem (myid, 0, send_buff, strlen(send_buff)+1);

    }
    else {

      printf ("task=%d vai enviar %s para task 0\n", myid, send_buff);
      enviarMensagem (myid, 0, send_buff, strlen(send_buff)+1);

      printf ("task=%d recebeu %s da task 0\n", myid, receive_buff);
      receberMensagem (0, myid, receive_buff, BUFFSZ);

    }
   }

   thread->matrix = simul(thread);
   return 0;
}

/*--------------------------------------------------------------------
| Function:
| Description: cria n threads
|              para cada um dos filhos recebe uma mensagem e envia
               uma resposta n vezes
---------------------------------------------------------------------*/
argsSimular_t* createThreads(int numTarefas, int it, int k, DoubleMatrix2D *matrix, int t, int b, int e, int d) {

  argsSimular_t* slaves = (argsSimular_t*) malloc(numTarefas * sizeof(argsSimular_t));
  /* create slaves */
  int h = 0;
  for (int i = 0; i < numTarefas; i++) {

    slaves[i].matrix = malloc(sizeof(double*) * (k + 2));
    for(int a = 0; a < k + 2; a++)
      slaves[i].matrix[a] = malloc((2 + k) * numTarefas * sizeof(double));

    int line = 0;
    for(int a = h; a < h + k + 1; a++) {

      slaves[i].matrix[line][0] = (double) e;
      slaves[i].matrix[line][-1] = (double) d;

        for(int col = 1; col < numTarefas * k; col++) {

          if(h == 0 && line == 0 && line <= numTarefas * k)
            slaves[i].matrix[line][col] = (double) t;

          else if(line > numTarefas * k)
            slaves[i].matrix[-1][col] = (double) b;

          else
            slaves[i].matrix[line][col] = dm2dGetEntry(matrix, a, col);
      }
      line++;
    }
    h += k;

    slaves[i].idx = i;
    slaves[i].iteracoes = it;
    slaves[i].id = pthread_create(&slaves[i], NULL, slaveThread, &slaves[i]);

  }
  return slaves;

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
