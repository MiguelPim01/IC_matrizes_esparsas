#ifndef MATRIZ_H
#define MATRIZ_H

typedef struct Matriz Matriz;

typedef struct Valor Valor;

typedef struct Vetor Vetor;

Vetor *vector_construct(int size);

Vetor *vector_read_txt(char *filePath);

void vector_print_esparso(Vetor *v);

void vector_destroy(Vetor *v);


Matriz *matriz_construct(int qtd_nnz, int qtdLinhas, int qtdColunas);

void matriz_add_value(Matriz *matriz, Valor v);

Matriz *matriz_read_mtx(char *filePath);

Vetor *matriz_multiply_by_vector(Matriz *matriz, Vetor *vector);

void matriz_print_esparso(Matriz *m);

void matriz_destroy(Matriz *m);

int matriz_qtd_colunas(Matriz *m);

#endif