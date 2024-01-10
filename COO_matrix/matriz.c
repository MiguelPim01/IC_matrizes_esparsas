#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matriz.h"

struct Vetor {
    float *array;
    int size;
};

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

/* ============ VETOR FUNCTIONS ============ */

Vetor *vector_construct(int size)
{
    Vetor *v = (Vetor *)malloc(sizeof(Vetor));

    v->array = (float *)malloc(sizeof(float) * size);
    v->size = size;

    for (int i = 0; i < size; i++)
        v->array[i] = 0.0;
    
    return v;
}

Vetor *vector_read_txt(char *filePath)
{
    FILE *file = fopen(filePath, "r");

    int qtd_nnz, qtdLinhas, qtdColunas;

    fscanf(file, "%d %d %d", &qtdLinhas, &qtdColunas, &qtd_nnz);

    Vetor *v = (Vetor *)malloc(sizeof(Vetor));

    v->array = (float *)malloc(sizeof(float) * qtdLinhas);
    v->size = qtdLinhas;

    int linha, coluna;
    float valor;

    while (!feof(file))
    {
        fscanf(file, "%d %d %f", &linha, &coluna, &valor);

        v->array[linha-1] = valor;
    }

    fclose(file);

    return v;
}

void vector_print_esparso(Vetor *v)
{
    if (v == NULL)
        return;

    for (int i = 0; i < v->size; i++)
    {
        printf("(%d, 1): %.4f\n", i+1, v->array[i]);
    }
}

void vector_destroy(Vetor *v)
{
    if (v == NULL)
        return;

    free(v->array);
    free(v);
}


/* ============ VALOR FUNCTIONS ============ */

int _cmp_valor_position(Valor v1, Valor v2)
{
    return (v1.linha == v2.linha && v1.coluna == v2.coluna);
}

Valor _mult_valores(Valor v1, Valor v2)
{
    Valor v;

    v.linha = v1.linha;
    v.coluna = 1;
    v.valor = v1.valor * v2.valor;

    return v;
}

/* ============ MATRIZ FUNCTIONS ============ */

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
    if (m->size == m->qtd_nnz)
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

Vetor *matriz_multiply_by_vector(Matriz *m, Vetor *v)
{
    if (m->qtdColunas != v->size)
        return NULL;
    
    Vetor *resultado = vector_construct(m->qtdLinhas);

    for (int i = 0; i < m->qtd_nnz; i++) // Analisa todos os nnz da matriz
    {
        resultado->array[m->valores[i].linha-1] += v->array[m->valores[i].coluna-1] * m->valores[i].valor;
    }

    return resultado;
}

void matriz_print_esparse(Matriz *m)
{
    if (m == NULL)
        return;

    for (int i = 0; i < m->size; i++)
        printf("(%d, %d): %.4f\n", m->valores[i].linha, m->valores[i].coluna, m->valores[i].valor);
}

void matriz_destroy(Matriz *m)
{
    if (m == NULL)
        return;

    free(m->valores);
    free(m);
}


int matriz_qtd_colunas(Matriz *m)
{
    return m->qtdColunas;
}