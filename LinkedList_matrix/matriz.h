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

int matriz_get_ordem(Matriz *m);

void matriz_print_esparso(Matriz *m);

void matriz_destroy(Matriz *m);

/**
 * Realiza a fatoração LU incompleta com level de fill in p, da matriz m nas matrizes L e U.
 * 
 * @param m matriz que será realizada a fatoração
 * 
 * @param L matriz triangular inferior da fatoração
 * 
 * @param U matriz triangular superior da fatoração
 * 
 * @param p level de fill in da fatoração
*/
void ilup(Matriz *m, Matriz *L, Matriz *U, int p);

#endif