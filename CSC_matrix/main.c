#include <stdio.h>
#include <stdlib.h>

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

    Matriz *resultado = matriz_multiply_by_vector(m, vetor);

    matriz_print_esparso(m);
    printf("\n");
    matriz_print_esparso(vetor);
    printf("\n");
    matriz_print_esparso(resultado);

    matriz_destroy(resultado);
    matriz_destroy(m);
    matriz_destroy(vetor);

    return 0;
}
