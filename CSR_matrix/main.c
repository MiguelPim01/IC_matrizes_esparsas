#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "matriz.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
        exit(printf("Argumentos insuficientes!\n"));

    Matriz *m = matriz_read_mtx(argv[1]);

    matriz_print_esparso(m);

    SparMAT *mat = NULL;

    CSR_to_SparMAT(m, mat);

    matriz_destroy(m);
    SparMAT_destroy(mat);

    return 0;
}
