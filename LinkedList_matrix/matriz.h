#ifndef MATRIZ_H
#define MATRIZ_H

typedef struct Matriz Matriz;

typedef struct LinkedList LinkedList;

typedef struct Node Node;

typedef struct Vetor Vetor;

Vetor *vector_read_txt(char *filePath);

void vector_print_esparso(Vetor *v);

void vector_destroy(Vetor *v);


Matriz *matriz_construct(int qtdLinhas, int qtdColunas, int qtd_nnz);

Vetor *matriz_multiply_by_vector(Matriz *m, Vetor *v);

Matriz *matriz_read_mtx(char *filePath);

void matriz_print_esparso(Matriz *m);

void matriz_destroy(Matriz *m);

#endif