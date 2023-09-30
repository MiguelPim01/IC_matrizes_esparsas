#ifndef MATRIZ_H
#define MATRIZ_H

typedef struct Matriz Matriz;

typedef struct Valor Valor;

Matriz *matriz_construct(int qtd_nnz, int qtdLinhas, int qtdColunas);

void matriz_add_value(Matriz *matriz, Valor v);

Matriz *matriz_read_mtx(char *filePath);

Matriz *matriz_read_txt(char *filePath);

Matriz *matriz_multiply_by_vector(Matriz *matriz, Matriz *vector);

void matriz_print_esparse(Matriz *m);

void matriz_destroy(Matriz *m);


int matriz_qtd_colunas(Matriz *m);

#endif