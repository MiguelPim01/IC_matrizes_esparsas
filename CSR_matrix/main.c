#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "matriz.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
        exit(printf("Argumentos insuficientes!\n"));

    Matriz *m = matriz_read_mtx(argv[1]);

    printf("MATRIZ (A):\n");
    matriz_print_esparso(m);
    printf("\n");


    SparMAT *mat = (SparMAT *)malloc(sizeof(SparMAT));
    CSR_to_SparMAT(m, mat);

    int n = matriz_get_ordem(m);

    SparILU *lu = (SparILU *)malloc(sizeof(SparILU));
    SPARILU_setup(lu, n);

    ilup(mat, lu, 1);

    Matriz *L = matriz_build(), *U = matriz_build();

    SparILU_to_CSR(lu, L, U);

    printf("MATRIZ (L):\n");
    matriz_print_esparso(L);
    printf("\n");
    printf("MATRIZ (U):\n");
    matriz_print_esparso(U);
    printf("\n");

    matriz_destroy(L);
    matriz_destroy(U);
    SparILU_destroy(lu);
    SparMAT_destroy(mat);
    matriz_destroy(m);

    return 0;
}
