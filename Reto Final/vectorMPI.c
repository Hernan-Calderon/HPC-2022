#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

int *vector;

void imprimir_vector(int *vector, int n) {
  for (int i = 0; i < n; i++) {
    printf("\t%d", vector[i]);
  }
}

int main(int argc, char *argv[]) {

  int rank, size, n;
  int to, rows, workload, rleft, offset;
  int i, j;
  struct timeval start, end;
  long double exec_time;
  char *ptr;
  int N = strtol(argv[1], &ptr, 10);
  int iteraciones = strtol(argv[2], &ptr, 10);

  // Reserva de Memoria
  vector = (int *)malloc(N * sizeof(int));

  // Damos Valores al vector
  srand(time(NULL));

  for (int i = 0; i < N; i++) {
    vector[i] = rand() % 2;
  }

  /* inicializamos MPI */
  MPI_Status status;
  MPI_Init(NULL, NULL);

  /* Identificador de procesos */
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  /* Numero de procesos */
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  /* Nodos */
  char node[MPI_MAX_PROCESSOR_NAME];
  MPI_Get_processor_name(node, &n);

  /* head node */
  if (rank == 0) {

    /* inicia el temporizador */
    gettimeofday(&start, NULL);

    workload = N / (size - 1);
    rleft = N % (size - 1);
    offset = 0;

    for (to = 1; to < size; to++) {
      if (to > rleft) {
        rows = workload;
      } else {
        rows = workload + 1;
      }

      /* Se envia la carga de trabajo a cada nodo */
      MPI_Send(&rows, 1, MPI_INT, to, 1, MPI_COMM_WORLD);
      MPI_Send(&offset, 1, MPI_INT, to, 1, MPI_COMM_WORLD);
      for (j = offset; j < offset + rows; j++) {
        MPI_Send(&vector[j], N, MPI_INT, to, 1, MPI_COMM_WORLD);
      }
      offset = offset + rows;
    }

    /* Se reciben los resultados */
    for (i = 1; i < size; i++) {
      MPI_Recv(&rows, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
      MPI_Recv(&offset, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
      for (j = offset; j < offset + rows; j++) {
        MPI_Recv(&vector[j], rows * N, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
      }
    }
    /* Finaliza el temporizador */
    gettimeofday(&end, NULL);
    exec_time = (((end.tv_sec - start.tv_sec) * 1000.0) +
                 ((end.tv_usec - start.tv_usec) / 1000.0));

    printf("** Result Array **\n");
    // imprimir_vector(vector,n);
    printf("Execution time = %Lf ms \n", exec_time);

  }

  /* compute node */
  else if (rank != 0) {

    printf("Receiver node %s with rank:%d\n", node, rank);

    /* Se recibe la carga de trabajo */
    MPI_Recv(&rows, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&offset, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    for (j = 0; j < rows; j++) {
      MPI_Recv(&vector[j], N, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    }
    /* Se realizan los calculos */
    for (int i = 0; i < iteraciones; i++) {
      for (int i = 0; i < n; i++) {
        if (i + 1 == n) {
          if (vector[i] == 1 && vector[0] == 0) {
            vector[i] = 0;
            vector[0] = 1;
          }
        } else {
          if (vector[i] == 1 && vector[i + 1] == 0) {
            vector[i] = 0;
            vector[i + 1] = 1;
          }
        }
      }
    }

    /* Se envian los resultados al head node */
    MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    for (j = 0; j < rows; j++) {
      MPI_Send(&vector[j], N, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }
  }

  /* Se termina MPI */
  MPI_Finalize();

  return 0;
}