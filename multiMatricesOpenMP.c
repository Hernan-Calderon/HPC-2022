#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
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

void calcular_producto(int n) {

#pragma omp parallel for
  for (int a = 0; a < n; a++) {
    for (int i = 0; i < n; i++) {
      int suma = 0;
      for (int j = 0; j < n; j++) {
        suma += matrizA[a][j] * matrizB[i][j];
      }
      producto[a][i] = suma;
    }
  }
}

int main(int argc, char *argv[]) {
  char *ptr;
  int n = strtol(argv[1], &ptr, 10);

  // Reserva de Memoria
  matrizA = (int **)malloc(n * sizeof(int *));
  matrizB = (int **)malloc(n * sizeof(int *));
  producto = (int **)malloc(n * sizeof(int *));

  for (int i = 0; i < n; i++) {
    matrizA[i] = (int *)malloc(n * sizeof(int));
    matrizB[i] = (int *)malloc(n * sizeof(int));
    producto[i] = (int *)malloc(n * sizeof(int));
  }

  // Damos Valores a las Matrices
  srand(time(NULL));

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      matrizA[i][j] = rand() % 11;
      matrizB[i][j] = rand() % 11;
    }
  }

  clock_t inicio = clock();
  // Se calcula el producto de las matrices
  calcular_producto(n);

  clock_t fin = clock();
  double time_spent = ((double)(fin - inicio) / CLOCKS_PER_SEC);
  printf("tiempo = %.6f\n", time_spent);

  // imprimir_matriz(matrizA,n);
  // imprimir_matriz(matrizB,n);
  // imprimir_matriz(producto,n);
  return 0;
}