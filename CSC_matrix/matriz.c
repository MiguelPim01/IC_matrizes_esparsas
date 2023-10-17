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

struct Valor {
    float valor;
    int linha;
};

int _cmp_valor(Valor v1, Valor v2)
{
    return (v1.linha == v2.linha);
}

Valor _mult_valores(Valor v1, Valor v2)
{
    Valor v;

    v.linha = v1.linha;
    v.valor = v1.valor * v2.valor;

    return v;
}

struct Matriz {
    Valor *valores;
    int *ptr_coluna;
    int qtd_nnz, qtdLinhas, qtdColunas;
    int size;
};

Matriz *matriz_construct(int qtd_nnz, int qtdLinhas, int qtdColunas)
{
    Matriz *m = (Matriz *)malloc(sizeof(Matriz));

    m->valores = (Valor *)calloc(qtd_nnz, sizeof(Valor));
    m->ptr_coluna = (int *)malloc(sizeof(int) * (qtdColunas+1));

    for (int i = 0; i < qtdColunas; i++)
        m->ptr_coluna[i] = -1;
    m->ptr_coluna[qtdColunas] = qtd_nnz;

    m->qtd_nnz = qtd_nnz;
    m->qtdLinhas = qtdLinhas;
    m->qtdColunas = qtdColunas;
    m->size = 0;

    return m;
}

Matriz *vetor_construct(int qtd_nnz)
{
    Matriz *v = (Matriz *)malloc(sizeof(Matriz));

    v->valores = (Valor *)malloc(sizeof(Valor) * qtd_nnz);
    v->ptr_coluna = (int *)malloc(sizeof(int) * 2);

    v->qtd_nnz = qtd_nnz;
    v->qtdLinhas = qtd_nnz;
    v->qtdColunas = 1;
    v->size = qtd_nnz;

    v->ptr_coluna[0] = 0; v->ptr_coluna[1] = qtd_nnz;

    for (int i = 0; i < qtd_nnz; i++)
    {
        v->valores[i].linha = i+1;
        v->valores[i].valor = 0;
    }

    return v;
}

void _vetor_add_value(Matriz *vetor, Valor v)
{
    vetor->valores[v.linha-1].valor += v.valor;
}

void matriz_add_value(Matriz *m, Valor v, int coluna)
{
    int ind_coluna = m->ptr_coluna[coluna-1];

    m->ptr_coluna[coluna-1] = (ind_coluna == -1) ? m->size : ind_coluna;

    if (m->size == m->qtd_nnz)
        return;

    m->valores[m->size] = v;
    m->size++;
}

Matriz *matriz_multiply_by_vector(Matriz *m, Matriz *vetor)
{
    if (m->qtdColunas != vetor->qtdLinhas)
        return NULL;
    
    Matriz *resultado = vetor_construct(m->qtdLinhas);

    Valor v_m, v_vetor, v_resultado;

    for (int i = 0; i < m->qtdColunas; i++)
    {
        for (int k = m->ptr_coluna[i]; k < m->ptr_coluna[i+1]; k++)
        {
            v_m = m->valores[k];
            v_vetor = vetor->valores[i];

            v_resultado = _mult_valores(v_m, v_vetor);
            _vetor_add_value(resultado, v_resultado);
        }
    }

    return resultado;
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
    int coluna;

    while (!feof(file)) // fase 3: Ler os valores da matriz
    {
        fread(&c, sizeof(char), 1, file);

        flag = (c == '\n') ? 1 : 0;

        linha[string_size] = c;
        linha[++string_size] = '\0';

        if (flag)
        {
            sscanf(linha, "%d %d %f", &v.linha, &coluna, &v.valor);

            matriz_add_value(m, v, coluna);
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
    int coluna;

    while (!feof(file))
    {
        fscanf(file, "%d %d %f", &v.linha, &coluna, &v.valor);

        matriz_add_value(m, v, coluna);
    }

    fclose(file);

    return m;
}

void matriz_print_esparso(Matriz *m)
{
    if (m == NULL)
        return;
    
    for (int i = 0; i < m->qtdColunas; i++)
        for (int k = m->ptr_coluna[i]; k < m->ptr_coluna[i+1]; k++)
            printf("(%d, %d): %.3f\n", m->valores[k].linha, i+1, m->valores[k].valor);
}

void matriz_destroy(Matriz *m)
{
    if (m == NULL)
        return;

    free(m->ptr_coluna);
    free(m->valores);
    free(m);
}