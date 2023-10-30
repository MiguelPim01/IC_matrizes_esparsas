#include <stdio.h>
#include <stdlib.h>

#include "matriz.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
        exit(printf("ERRO: Argumentos insuficientes!\n"));

    Matriz *m = matriz_read_mtx(argv[1]);

    Vetor *v = vector_read_txt(argv[2]);

    matriz_print_esparso(m);
    printf("\n");
    vector_print_esparso(v);

    matriz_destroy(m);
    vector_destroy(v);

    return 0;
}
