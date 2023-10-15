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
    int coluna;
};

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

struct Matriz {
    Valor *valores;
    int *ptr_linha;
    int qtd_nnz, qtdLinhas, qtdColunas;
    int size;
};

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

void _mult_vetor_add_value(Matriz *vetor, Valor v)
{
    for (int k = 0; k < vetor->qtdLinhas; k++)
    {
        for (int i = vetor->ptr_linha[k]; i < vetor->ptr_linha[k+1] && i != -1; i++)
        {
            if (_cmp_valor(vetor->valores[i], v))
            {
                vetor->valores[i].valor += v.valor;
                return;
            }
        }
    }

    vetor->ptr_linha[0] = 0;
    vetor->valores[vetor->size] = v;
    vetor->size++;
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

Matriz *matriz_multiply_by_vector(Matriz *m, Matriz *vetor)
{
    if (m->qtdColunas != vetor->qtdLinhas)
        return NULL;
    
    Matriz *resultado = matriz_construct(m->qtdLinhas, m->qtdLinhas, 1);

    Valor v_m, v_vetor, v_resultado;

    for (int i = 0; i < m->qtdLinhas; i++)
    {
        for (int k = m->ptr_linha[i]; k < m->ptr_linha[i+1]; k++)
        {
            v_m = m->valores[k];
            for (int j = 0; j < vetor->qtdLinhas; j++)
            {
                v_vetor = m->valores[j];
                if (v_m.coluna == j+1)
                {   
                    v_resultado = _mult_valores(v_m, v_vetor);
                    _mult_vetor_add_value(resultado, v_resultado);
                }
            }
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

Matriz *matriz_read_txt(char *filePath)
{
    FILE *file = fopen(filePath, "r");

    int qtd_nnz, qtdLinhas, qtdColunas;

    fscanf(file, "%d %d %d", &qtdLinhas, &qtdColunas, &qtd_nnz);

    Matriz *m = matriz_construct(qtd_nnz, qtdLinhas, qtdColunas);

    Valor v;
    int linha;

    while (!feof(file))
    {
        fscanf(file, "%d %d %f", &linha, &v.coluna, &v.valor);

        matriz_add_value(m, v, linha);
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
            printf("(%d, %d): %.3f\n", i+1, m->valores[k].coluna, m->valores[k].valor);
}

void matriz_destroy(Matriz *m)
{
    if (m == NULL)
        return;

    free(m->ptr_linha);
    free(m->valores);
    free(m);
}