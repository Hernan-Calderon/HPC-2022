#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

int **matrizA;
int **matrizB;
int **producto;


void imprimir_matriz(int **matriz, int n) {
  for (int i = 0; i < n; i++) {
    printf("\n");
    for (int j = 0; j < n; j++)
      printf("\t%d", matriz[i][j]);
  }
  printf("\n");
}


int main (int argc, char *argv[]){

	int rank, size, n;
	int to,rows, workload, rleft, offset;
	int i, j, k;
	struct timeval start, end;
	long double exec_time;
  char *ptr;
  int N = strtol(argv[1], &ptr, 10);

  // Reserva de Memoria
  matrizA = (int **)malloc(N * sizeof(int *));
  matrizB = (int **)malloc(N * sizeof(int *));
  producto = (int **)malloc(N * sizeof(int *));

  for (int i = 0; i < n; i++) {
    matrizA[i] = (int *)malloc(N * sizeof(int));
    matrizB[i] = (int *)malloc(N * sizeof(int));
    producto[i] = (int *)malloc(N * sizeof(int));
  }	

	// Damos Valores a las Matrices
  srand(time(NULL));

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      matrizA[i][j] = rand() % 11;
      matrizB[i][j] = rand() % 11;
    }
  }
	
	/* inicializamos MPI */
	MPI_Status status;
	MPI_Init(NULL,NULL);

	/* Identificador de procesos */
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	
	/* Numero de procesos */
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	
	/* Nodos */
	char node[MPI_MAX_PROCESSOR_NAME];
	MPI_Get_processor_name(node, &n);
	
	/* Se transmite la segunda matriz a todos los nodos*/	 
	for(i=0;i<N;i++){
		MPI_Bcast(&matrizB[i][0],N,MPI_INT,0,MPI_COMM_WORLD);
	}
	

	/* head node */
	if (rank == 0){
		
		/* inicia el temporizador */
		gettimeofday(&start, NULL);

		workload = N/(size-1);
		rleft = N%(size-1);
		offset = 0;
		
		for (to=1; to < size; to++){
			if (to > rleft){
			rows = workload;
			}
			else{ 
			rows = workload+1;
			}

			/* Se envia la carga de trabajo a cada nodo */
			MPI_Send(&rows, 1, MPI_INT, to, 1, MPI_COMM_WORLD);
			MPI_Send(&offset, 1, MPI_INT, to, 1, MPI_COMM_WORLD);
			for(j=offset;j<offset+rows;j++){
				MPI_Send(&matrizA[j][0], N, MPI_INT, to, 1, MPI_COMM_WORLD);
			}
			offset = offset + rows;
		}

		/* Se reciben los resultados */
		for (i=1; i<size; i++){
			MPI_Recv(&rows, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
			MPI_Recv(&offset, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
			for(j=offset;j<offset+rows;j++){
				MPI_Recv(&producto[j][0], rows*N, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
			}
		}
		/* Finaliza el temporizador */
		gettimeofday(&end, NULL);
		exec_time = (((end.tv_sec - start.tv_sec) * 1000.0 )+ ((end.tv_usec - start.tv_usec) / 1000.0));

		printf("** Result Array **\n");
		// imprimir_matriz(producto,n);
		printf("Execution time = %Lf ms \n",exec_time);

	}

	/* compute node */
	else if(rank != 0){

		printf("Receiver node %s with rank:%d\n",node,rank);
		
		/* Se recibe la carga de trabajo */
		MPI_Recv(&rows, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&offset, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		for(j=0;j<rows;j++){
			MPI_Recv(&matrizA[j][0],N, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		}

		/* Se realizan los calculos */
		k=0;
		while(k<N){
			for (i=0; i<rows; i++){
				producto[i][k] = 0;
				for (j=0; j<N; j++){
					producto[i][k] = producto[i][k] + matrizA[i][j] * matrizB[j][k];
				}
			}
			k++;
		}

		/* Se envian los resultados al head node */
		MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
		MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
		for(j=0;j<rows;j++){
			MPI_Send(&producto[j][0], N, MPI_INT, 0, 2, MPI_COMM_WORLD);
		}
	}

	/* Se termina MPI */
	MPI_Finalize();

	return 0;
}
