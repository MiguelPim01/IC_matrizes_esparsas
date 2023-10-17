#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matriz.h"

struct Valor {
    float valor;
    int linha;
    int coluna;
};

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

Matriz *vetor_construct(int qtd_nnz)
{
    Matriz *v = (Matriz *)malloc(sizeof(Matriz));

    v->valores = (Valor *)malloc(sizeof(Valor) * qtd_nnz);

    v->qtd_nnz = qtd_nnz;
    v->qtdLinhas = qtd_nnz;
    v->qtdColunas = 1;
    v->size = qtd_nnz;

    for (int i = 0; i < v->qtdLinhas; i++)
    {
        v->valores[i].valor = 0;
        v->valores[i].coluna = 1;
        v->valores[i].linha = i+1;
    }

    return v;
}

void _vetor_add_value(Matriz *vetor, Valor v)
{
    vetor->valores[v.linha-1].valor += v.valor;
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
    if (m->qtdColunas != vetor->qtdLinhas)
        return NULL;
    
    Matriz *resultado = vetor_construct(m->qtdLinhas);

    Valor valor_resultado;
    Valor v_m, v_vetor;

    for (int i = 0; i < m->qtd_nnz; i++) // Analisa todos os nnz da matriz
    {
        v_m = m->valores[i];
        v_vetor = vetor->valores[v_m.coluna-1];

        valor_resultado = _mult_valores(v_m, v_vetor);
        _vetor_add_value(resultado, valor_resultado);
    }

    return resultado;
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


int matriz_qtd_colunas(Matriz *m)
{
    return m->qtdColunas;
}