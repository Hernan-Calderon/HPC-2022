#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int **matrizA;
int **matrizB;
int **producto;
int n;
int filas_hilo;
int num_hilos;
#define HILOS 2;

void imprimir_matriz(int **matriz, int n) {
  for (int i = 0; i < n; i++) {
    printf("\n");
    for (int j = 0; j < n; j++)
      printf("\t%d", matriz[i][j]);
  }
  printf("\n");
}

void *ProductoHilo(void *t) {
  
  int c = (long)t;
  int inicio = filas_hilo * c;
  int fin;
  if (c < num_hilos - 1) {
    fin = filas_hilo + inicio;
  } else {
    fin = n;
  }  
  for (int a = 0; a < n; a++) {
    for (int i = inicio; i < fin; i++) {
      int suma = 0;
      for (int j = 0; j < n; j++) {
        suma += matrizA[i][j] * matrizB[j][a];        
      }
      producto[i][a] = suma;      
    }
  }
  pthread_exit((void *)t);
}

void calcular_producto(int n) {
  if (n <= num_hilos) {
    num_hilos = n;
    filas_hilo = 1;
  } else {
    num_hilos = HILOS;
    filas_hilo = ceil((float)n / (float)num_hilos);
  }

  pthread_t threads[num_hilos];
  pthread_attr_t attr;
  int rc;
  long t;
  void *status;

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  for (t = 0; t < num_hilos; t++) {
    rc = pthread_create(&threads[t], &attr, ProductoHilo, (void *)t);
    if (rc) {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }

  pthread_attr_destroy(&attr);
  for (t = 0; t < num_hilos; t++) {
    rc = pthread_join(threads[t], &status);
    if (rc) {
      printf("ERROR; return code from pthread_join() is %d\n", rc);
      exit(-1);
    }
  }
  
}

int main(int argc, char *argv[]) {
  char *ptr;
  n = strtol(argv[1], &ptr, 10);

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

  //imprimir_matriz(matrizA, n);
  //imprimir_matriz(matrizB, n);
  //imprimir_matriz(producto, n);
  pthread_exit(NULL);
  return 0;
}