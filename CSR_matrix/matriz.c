#include <stdio.h>
#include <stdlib.h>

#include "matriz.h"

/**
 * COMENTÁRIOS: 
 * 
 * -> No array "ptr_coluna" se o valor for -1 significa que não existem valores naquela coluna.
 * 
 * -> Todos os valores devem ser adicionados seguindo a ordem da primeira ate a ultima coluna.
 * 
 */

struct Vetor {
    float *array;
    int size;
};

struct Valor {
    float valor;
    int coluna;
};

struct Matriz {
    Valor *valores;
    int *ptr_linha;
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

int _cmp_valor(Valor v1, Valor v2)
{
    return (v1.coluna == v2.coluna);
}

Valor _mult_valores(Valor v1, Valor v2)
{
    Valor v;

    v.coluna = 1;
    v.valor = v1.valor * v2.valor;

    return v;
}

/* ============ MATRIZ FUNCTIONS ============ */

Matriz *matriz_construct(int qtd_nnz, int qtdLinhas, int qtdColunas)
{
    Matriz *m = (Matriz *)malloc(sizeof(Matriz));

    m->valores = (Valor *)calloc(qtd_nnz, sizeof(Valor));
    m->ptr_linha = (int *)malloc(sizeof(int) * (qtdLinhas+1));

    for (int i = 0; i < qtdLinhas; i++)
        m->ptr_linha[i] = -1;
    m->ptr_linha[qtdLinhas] = qtd_nnz;

    m->qtd_nnz = qtd_nnz;
    m->qtdLinhas = qtdLinhas;
    m->qtdColunas = qtdColunas;
    m->size = 0;

    return m;
}

void matriz_add_value(Matriz *m, Valor v, int linha)
{
    int ind_linha = m->ptr_linha[linha-1];

    m->ptr_linha[linha-1] = (ind_linha == -1) ? m->size : ind_linha;

    if (m->size == m->qtd_nnz)
        return;

    m->valores[m->size] = v;
    m->size++;
}

Vetor *matriz_multiply_by_vector(Matriz *m, Vetor *v)
{
    if (m->qtdColunas != v->size)
        return NULL;
    
    Vetor *resultado = vector_construct(m->qtdLinhas);

    for (int i = 0; i < m->qtdLinhas; i++)
    {
        for (int k = m->ptr_linha[i]; k < m->ptr_linha[i+1]; k++)
        {
            resultado->array[i] += v->array[m->valores[k].coluna-1] * m->valores[k].valor;
        }
    }

    return resultado;
}

Matriz *matriz_read_mtx(char *filePath) // irá ler a transposta da matriz
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
            sscanf(linha, "%d %d %d", &qtdColunas, &qtdLinhas, &qtd_nnz);
            string_size = 0;
            flag = 0;
            break;
        }
    }

    Matriz *m = matriz_construct(qtd_nnz, qtdLinhas, qtdColunas);

    Valor v;
    int l;

    while (!feof(file)) // fase 3: Ler os valores da matriz
    {
        fread(&c, sizeof(char), 1, file);

        flag = (c == '\n') ? 1 : 0;

        linha[string_size] = c;
        linha[++string_size] = '\0';

        if (flag)
        {
            sscanf(linha, "%d %d %f", &v.coluna, &l, &v.valor);

            matriz_add_value(m, v, l);
            flag = 0;
            string_size = 0;
        }
    }

    fclose(file);
    
    return m;
}

void matriz_print_esparso(Matriz *m)
{
    if (m == NULL)
        return;
    
    for (int i = 0; i < m->qtdLinhas; i++)
        for (int k = m->ptr_linha[i]; k < m->ptr_linha[i+1]; k++)
            printf("(%d, %d): %.4f\n", i+1, m->valores[k].coluna, m->valores[k].valor);
}

void matriz_destroy(Matriz *m)
{
    if (m == NULL)
        return;

    free(m->ptr_linha);
    free(m->valores);
    free(m);
}