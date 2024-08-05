#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "matriz.h"

int main(int argc, char *argv[])
{
    // if (argc < 3)
    //     exit(printf("ERRO: Argumentos insuficientes!\n"));

    // Matriz *m = matriz_read_mtx(argv[1]);

    // Vetor *v = vector_read_txt(argv[2]);

    // Vetor *mult = v;

    // clock_t start, end;
    // double total = 0;

    // for (int i = 0; i < 5; i++)
    // {
    //     start = clock();
    //     mult = matriz_multiply_by_vector(m, v);
    //     end = clock();

    //     total += (double)(end - start)/CLOCKS_PER_SEC;

    //     vector_destroy(v);
    //     v = mult;
    // }

    // // matriz_print_esparso(m);
    // // printf("\n");
    // // vector_print_esparso(v);
    // // printf("\n");

    // printf("%lf", total);

    // matriz_destroy(m);
    // vector_destroy(v);

    if (argc < 3) {
        exit(printf("ERRO: Argumentos insuficientes!\nRUN: ./main <matriz> <nivel_preenchimento>\n"));
    }

    clock_t start, end;

    Matriz *m = matriz_read_mtx(argv[1]);

    int n = matriz_get_ordem(m);
    int p;

    sscanf(argv[2], "%d", &p);

    Matriz *L = matriz_construct(n, n, 0);
    Matriz *U = matriz_construct(n, n, 0);

    start = clock();
    ilup(m, L, U, p);
    end = clock();

    // printf("Matriz A:\n");
    // matriz_print_esparso(m);
    // printf("\n");
    // printf("Matriz L:\n");
    // matriz_print_esparso(L);
    // printf("\n");
    // printf("Matriz U:\n");
    // matriz_print_esparso(U);
    // printf("\n");

    printf("%.6lf", (double)(end - start)/CLOCKS_PER_SEC);

    matriz_destroy(m);
    matriz_destroy(L);
    matriz_destroy(U);

    return 0;
}
