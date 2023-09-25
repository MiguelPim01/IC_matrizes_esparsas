#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "matriz.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
        exit(printf("Argumentos insuficientes, RUN: ./main <filePath>\n"));

    char caminho[100];

    sscanf(argv[1], "%s", caminho);

    Matriz *m = matriz_read_mtx(caminho);

    sscanf(argv[2], "%s", caminho);

    Matriz *vetor = matriz_read_txt(caminho);

    Matriz *resultado = matriz_multiply_by_vector_version1(m, vetor);

    matriz_print_esparse(m);
    printf("\n");
    matriz_print_esparse(vetor);
    printf("\n");
    matriz_print_esparse(resultado);

    matriz_destroy(m);
    matriz_destroy(vetor);
    matriz_destroy(resultado);

    return 0;
}