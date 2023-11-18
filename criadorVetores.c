#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "COO_matrix/matriz.h"

int main(int argc, char* argv[])
{
	if (argc < 2)
        exit(printf("Argumentos insuficientes!\n"));

    char nome_vetor[100], cat[5] = ".mtx";
    char caminho[100];

    sscanf(argv[1], "%s", caminho);

    sscanf(caminho, "matrizes/%*[^/]/%[^.]", nome_vetor);

    Matriz *m = matriz_read_mtx(caminho);

    int qtd_linhas_vetor = matriz_qtd_colunas(m);

    strcpy(caminho, "vetores/");
    strcat(caminho, nome_vetor);
    strcat(caminho, ".txt");

    FILE *file = fopen(caminho, "w");

    fprintf(file, "%d 1 %d\n", qtd_linhas_vetor, qtd_linhas_vetor);

    for (int i = 1; i <= qtd_linhas_vetor; i++)
    {
        fprintf(file, "%d 1 1\n", i);
    }

    fclose(file);

    matriz_destroy(m);

    return 0;
}