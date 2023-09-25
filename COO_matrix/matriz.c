#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matriz.h"

struct Valor {
    float valor;
    int linha;
    int coluna;
};

struct Matriz {
    Valor *valores;
    int qtd_nnz, qtdLinhas, qtdColunas;
    int size;
};

Matriz *matriz_construct(int qtd_nnz, int qtdLinhas, int qtdColunas)
{
    Matriz *m = (Matriz *)malloc(sizeof(Matriz));

    m->valores = (Valor *)calloc(qtd_nnz, sizeof(Valor));

    m->qtd_nnz = qtd_nnz;
    m->qtdColunas = qtdColunas;
    m->qtdLinhas = qtdLinhas;
    m->size = 0;

    return m;
}

void matriz_add_value(Matriz *m, Valor v)
{
    if (m->size == m->qtd_nnz || v.valor == 0)
        return;

    m->valores[m->size] = v;
    m->size++;
}

Matriz *matriz_read_mtx(char *filePath)
{
    FILE *file = fopen(filePath, "rb");

    if (!file)
        exit(printf("Nao foi possivel abrir o arquivo %s\n", filePath));

    char c;
    int flag = 0;

    while (!feof(file)) // fase 1: Ler os textos que não serao utilizados
    {
        fread(&c, sizeof(char), 1, file);

        if (flag && c != '%') { break; }

        if (c == '\n') { flag = 1; }
        else           { flag = 0; }
    }

    char linha[100];
    int string_size = 0;

    int qtd_nnz, qtdLinhas, qtdColunas;

    linha[string_size] = c;
    linha[++string_size] = '\0';

    flag = 0;
    while (!feof(file)) // fase 2: ler as dimensões da matriz
    {
        fread(&c, sizeof(char), 1, file);

        flag = (c == '\n') ? 1 : 0;

        linha[string_size] = c;
        linha[++string_size] = '\0';

        if (flag)
        {
            sscanf(linha, "%d %d %d", &qtdLinhas, &qtdColunas, &qtd_nnz);
            string_size = 0;
            flag = 0;
            break;
        }
    }

    Matriz *m = matriz_construct(qtd_nnz, qtdLinhas, qtdColunas);

    Valor v;

    while (!feof(file)) // fase 3: Ler os valores da matriz
    {
        fread(&c, sizeof(char), 1, file);

        flag = (c == '\n') ? 1 : 0;

        linha[string_size] = c;
        linha[++string_size] = '\0';

        if (flag)
        {
            sscanf(linha, "%d %d %f", &v.linha, &v.coluna, &v.valor);

            matriz_add_value(m, v);
            flag = 0;
            string_size = 0;
        }
    }

    fclose(file);
    
    return m;
}

Matriz *matriz_read_txt(char *filePath)
{
    FILE *file = fopen(filePath, "r");

    int qtd_nnz, qtdLinhas, qtdColunas;

    fscanf(file, "%d %d %d", &qtdLinhas, &qtdColunas, &qtd_nnz);

    Matriz *m = matriz_construct(qtd_nnz, qtdLinhas, qtdColunas);

    Valor v;

    while (!feof(file))
    {
        fscanf(file, "%d %d %f", &v.linha, &v.coluna, &v.valor);

        matriz_add_value(m, v);
    }

    fclose(file);

    return m;
}

Matriz *matriz_multiply_by_vector(Matriz *m, Matriz *vetor)
{
    // Para ser feita ainda

    return NULL;
}

void matriz_print_esparse(Matriz *m)
{
    if (m == NULL)
        return;

    for (int i = 0; i < m->size; i++)
        printf("(%d, %d): %.3f\n", m->valores[i].linha, m->valores[i].coluna, m->valores[i].valor);
}

void matriz_destroy(Matriz *m)
{
    if (m == NULL)
        return;

    free(m->valores);
    free(m);
}