
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

int *A;
int *B;
int *C;
int n;
int n_process=8;

// Inicializar la matriz din�mica
void initialarray(int *matrix)
{
    int i, j;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            *(matrix + i * n + j) = rand() % 11;
        }
    }
}

// Matriz de salida
void outputarray(int *matrix)
{
    int i, j;
    printf("Output the result:\n");
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            printf("%d ", *(matrix + i * n + j));
        }
        printf("\n");
    }
}

void multiplyRow(int start_index, int final_index)
{
    int i, j, k;
    for (i = 0; i < n; i++)
    {
        for (j = start_index; j < final_index; j++)
        {
            for (k = 0; k < n; k++)
            {
                *(C + i * n + j) += *(A + i * n + k) * *(A + k * n + j);
            }
        }
    }
}

void *create_shared_memory(size_t n)
{
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
    return mmap(NULL, n, protection, visibility, -1, 0);
}
//Programa principal
int main()
{
    // Arreglar un
    printf("procesos: %d\n",n_process);
    printf("Ingrese el n de filas y columnas\n");
    srand(time(NULL));
    scanf("%d", &n);

    A = create_shared_memory(n * n * sizeof(int));
    B = create_shared_memory(n * n * sizeof(int));
    C = create_shared_memory(n * n * sizeof(int));

    initialarray(A);
    //outputarray(A);
    initialarray(B);
    //outputarray(B);
    
    int i, pid, start_index, final_index, rows_per_proces, status;

    rows_per_proces = n / n_process;
    clock_t begin = clock();//Iniciar el tiempo
    for (i = 0; i < n_process; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            start_index = rows_per_proces * i;
            final_index = rows_per_proces * (i + 1);
            if (i == n_process - 1)
            {
                final_index = n;
            }
            multiplyRow(start_index, final_index);
            // outputarray(size, C);
            exit(0);
        }
    }


    while (n_process > 0)
    {
        pid = wait(&status);
        --n_process;
    }

    clock_t end = clock();//Finalizar el tiempo
    //outputarray(C);

    double time_spent = ((double)(end - begin) / CLOCKS_PER_SEC);

    printf("tiempo = %.5f\n", time_spent);

    return 0;
}