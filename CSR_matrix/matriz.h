#ifndef MATRIZ_H
#define MATRIZ_H

typedef struct Matriz Matriz;

typedef struct Valor Valor;

typedef struct Vetor Vetor;

typedef struct
{
	int           n; /* ordem da matriz                           */
	int*    nzcount; /* quantidade de não nulos de cada linha     */
	int**        ja; /* matriz com colunas de cada nnz por linha  */
	float**     ma;  /* matriz com valor de cada nnz por linha    */
} SparMAT;

typedef struct
{
	int         n;
	SparMAT*    L;
	float*     D;
	SparMAT*    U;
	int*     work;
} SparILU;

void CSR_to_SparMAT(Matriz *csr, SparMAT *mat);

void SparILU_to_CSR(SparILU *lu, Matriz *L, Matriz *U);

void SparMAT_destroy(SparMAT *mat);


Vetor *vector_construct(int size);

Vetor *vector_read_txt(char *filePath);

void vector_print_esparso(Vetor *v);

void vector_destroy(Vetor *v);


Matriz *matriz_construct(int qtd_nnz, int qtdLinhas, int qtdColunas);

void matriz_add_value(Matriz *m, Valor v, int linha);

Vetor *matriz_multiply_by_vector(Matriz *m, Vetor *v);

Matriz *matriz_read_mtx(char *filePath);

// Matriz *matriz_read_txt(char *filePath);

void matriz_print_esparso(Matriz *m);

void matriz_destroy(Matriz *m);

/**
 * ilup faz a fatoração LU em si.
 *
 * Nessa função não será necessário utilizar o level de fill in pois já foram computados os preenchimentos anteriormente.
 * 
 * @param m matriz que será fatorizada em L e U
 * 
 * @param L matriz triangular inferior da fatoração
 * 
 * @param U matriz triangular superior da fatoração
 * 
 * @param p level de fill in
 */
void ilup(SparMAT *m, SparILU *lu, int p);

#endif