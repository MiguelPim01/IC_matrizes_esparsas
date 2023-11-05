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

void matriz_add_value(Matriz *m, Valor v, int linha);

Vetor *matriz_multiply_by_vector(Matriz *m, Vetor *v);

Matriz *matriz_read_mtx(char *filePath);

Matriz *matriz_read_txt(char *filePath);

void matriz_print_esparso(Matriz *m);

void matriz_destroy(Matriz *m);

#endif