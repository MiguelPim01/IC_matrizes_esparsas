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

    clock_t tempo_total;

    clock_t start = clock();
    Matriz *resultado = matriz_multiply_by_vector(m, vetor);
    clock_t end = clock();

    matriz_print_esparso(resultado);

    tempo_total = (end - start);

    Matriz *aux;

    for (int i = 0; i < 5; i++)
    {
        start = clock();
        aux = matriz_multiply_by_vector(m, resultado);
        end = clock();

        matriz_destroy(resultado);
        resultado = aux;

        tempo_total += (end - start);
    }

    matriz_destroy(aux);

    printf("time spent: %lf\n", (double)tempo_total/CLOCKS_PER_SEC);

    matriz_destroy(m);
    matriz_destroy(vetor);

    return 0;
}
