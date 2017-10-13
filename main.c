 /*
// Projeto SO - exercicio 1, version 03
// Sistemas Operativos, DEI/IST/ULisboa 2017-18
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include "matrix2d.h"
#include "client_server_1.c"
#include "mplib3.h"
/*--------------------------------------------------------------------
| Function: simul
---------------------------------------------------------------------*/


DoubleMatrix2D *simul(DoubleMatrix2D *matrix, int linhas, int colunas, int numIteracoes) {
  DoubleMatrix2D *m, *aux, *tmp;
  int iter, i, j;
  double value;


  if(linhas < 2 || colunas < 2)
    return NULL;

  m = matrix;

  for (iter = 0; iter < numIteracoes; iter++) {

    for (i = 1; i < linhas - 1; i++)
      for (j = 1; j < colunas - 1; j++) {
        value = ( dm2dGetEntry(m, i-1, j) + dm2dGetEntry(m, i+1, j) +
		            dm2dGetEntry(m, i, j-1) + dm2dGetEntry(m, i, j+1) ) / 4.0;
        dm2dSetEntry(aux, i, j, value);
      }

    tmp = aux;
    aux = m;
    m = tmp;
  }

  return m;
}

/*--------------------------------------------------------------------
| Function: parse_integer_or_exit
---------------------------------------------------------------------*/

int parse_integer_or_exit(char const *str, char const *name)
{
  int value;

  if(sscanf(str, "%d", &value) != 1) {
    fprintf(stderr, "\nErro no argumento \"%s\".\n\n", name);
    exit(1);
  }
  return value;
}

/*--------------------------------------------------------------------
| Function: parse_double_or_exit
---------------------------------------------------------------------*/

double parse_double_or_exit(char const *str, char const *name)
{
  double value;

  if(sscanf(str, "%lf", &value) != 1) {
    fprintf(stderr, "\nErro no argumento \"%s\".\n\n", name);
    exit(1);
  }
  return value;
}


/*--------------------------------------------------------------------
| Function: main
---------------------------------------------------------------------*/

int main (int argc, char** argv) {

  if(argc != 9 ) {
    fprintf(stderr, "\nNumero invalido de argumentos.\n");
    fprintf(stderr, "Uso: heatSim N tEsq tSup tDir tInf iteracoes tarefas csz\n\n");
    return 1;
  }

  /* argv[0] = program name */
  int N = parse_integer_or_exit(argv[1], "N");
  double tEsq = parse_double_or_exit(argv[2], "tEsq");
  double tSup = parse_double_or_exit(argv[3], "tSup");
  double tDir = parse_double_or_exit(argv[4], "tDir");
  double tInf = parse_double_or_exit(argv[5], "tInf");
  int iteracoes = parse_integer_or_exit(argv[6], "iteracoes");
  int trab = parse_integer_or_exit(argv[7], "tarefas");
  int csz = parse_integer_or_exit(argv[8], "csz");

  DoubleMatrix2D *matrix, *result;

  fprintf(stderr, "\nArgumentos:\n"
	" N=%d tEsq=%.1f tSup=%.1f tDir=%.1f tInf=%.1f iteracoes=%d tarefas=%d csz=%d\n",
	N, tEsq, tSup, tDir, tInf, iteracoes, trab, csz);

  if(N < 1 || tEsq < 0 || tSup < 0 || tDir < 0 || tInf < 0 || iteracoes < 1 || N%trab != 0 || csz < 0) {
    fprintf(stderr, "\nErro: Argumentos invalidos.\n"
	" Lembrar que N >= 1, temperaturas >= 0, iteracoes >= 1, csz > 0 \n\n");
    return 1;
  }

  matrix = dm2dNew(N+2, N+2);
  argsSimular_t * slave_args = createThreads(trab, iteracoes, N/trab, matrix);

  if (matrix == NULL) {
    fprintf(stderr, "\nErro: Nao foi possivel alocar memoria para as matrizes.\n\n");
    return -1;
  }

  int i;

  for(i=0; i<N+2; i++)
    dm2dSetLineTo(matrix, i, 0);

  dm2dSetLineTo (matrix, 0, tSup);
  dm2dSetLineTo (matrix, N+1, tInf);
  dm2dSetColumnTo (matrix, 0, tEsq);
  dm2dSetColumnTo (matrix, N+1, tDir);

  result = simul(matrix, N+2, N+2, iteracoes);
  if (result == NULL) {
    printf("\nErro na simulacao.\n\n");
    return -1;
  }

  dm2dPrint(result);

  dm2dFree(matrix);

  return 0;
}
