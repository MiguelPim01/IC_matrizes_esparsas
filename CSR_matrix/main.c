#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "matriz.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
        exit(printf("Argumentos insuficientes!\nRUN: ./main <matriz> <nivel_preenchimento>\n"));

    clock_t start, end;

    Matriz *m = matriz_read_mtx(argv[1]);

    SparMAT *mat = (SparMAT *)malloc(sizeof(SparMAT));
    CSR_to_SparMAT(m, mat);

    int n = matriz_get_ordem(m);
    int p;

    sscanf(argv[2], "%d", &p);

    SparILU *lu = (SparILU *)malloc(sizeof(SparILU));
    SPARILU_setup(lu, n);

    start = clock();
    ilup(mat, lu, p);
    end = clock();

    Matriz *L = matriz_build(), *U = matriz_build();

    SparILU_to_CSR(lu, L, U);
    
    // printf("MATRIZ (A):\n");
    // matriz_print_esparso(m);
    // printf("\n");
    // printf("MATRIZ (L):\n");
    // matriz_print_esparso(L);
    // printf("\n");
    // printf("MATRIZ (U):\n");
    // matriz_print_esparso(U);
    // printf("\n");

    printf("%.4lf\n", (double)(end - start)/CLOCKS_PER_SEC);

    matriz_destroy(L);
    matriz_destroy(U);
    SparILU_destroy(lu);
    SparMAT_destroy(mat);
    matriz_destroy(m);

    return 0;
}
