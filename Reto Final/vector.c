#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int *vector;

void imprimir_vector(int *vector, int n) {
  for (int i = 0; i < n; i++) {
    printf("\t%d", vector[i]);
  }
}

void recorrer_vector(int *vector, int n) {  
  for (int i = 0; i < n; i++) {
    if(i+1 == n){
      if(vector[i]==1 && vector[0]==0){
        vector[i]=0;
        vector[0]=1;
      }
    }
    else{
      if(vector[i]==1 && vector[i+1]==0){
        vector[i]=0;
        vector[i+1]=1;
      }
    }
  }
  
}

int main(int argc, char *argv[]) {
  char *ptr;
  int n = strtol(argv[1], &ptr, 10);
  int iteraciones = strtol(argv[2], &ptr, 10);

  // Reserva de Memoria
  vector = (int *)malloc(n * sizeof(int));
  
  // Damos Valores al vector
  srand(time(NULL));

  for (int i = 0; i < n; i++) {
    vector[i] = rand() % 2;    
  }

  //imprimir_vector(vector, n);

  clock_t inicio = clock();
  for(int i=0; i<iteraciones; i++){
    //printf("\n");
    recorrer_vector(vector, n);
    //imprimir_vector(vector, n);    
  }
  clock_t fin = clock();
  double time_spent = ((double)(fin - inicio) / CLOCKS_PER_SEC);
  printf("tiempo = %.6f\n", time_spent);

  return 0;
}