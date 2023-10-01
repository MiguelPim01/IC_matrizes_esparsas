#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "matriz.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
        exit(printf("Argumentos insuficientes!\n"));

    char caminho[100];

    sscanf(argv[1], "%s", caminho);

    Matriz *m = matriz_read_mtx(caminho);

    sscanf(argv[2], "%s", caminho);

    Matriz *vetor = matriz_read_txt(caminho);

    clock_t start = clock();
    Matriz *resultado = matriz_multiply_by_vector(m, vetor);
    clock_t end = clock();

    printf("Time spent: %lf\n", (double)(end - start)/CLOCKS_PER_SEC);

    matriz_destroy(resultado);
    matriz_destroy(m);
    matriz_destroy(vetor);

    return 0;
}
