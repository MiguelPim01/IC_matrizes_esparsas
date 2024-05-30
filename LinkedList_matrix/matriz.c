#include <stdio.h>
#include <stdlib.h>

#include "matriz.h"

#define min(a, b) ((a < b) ? a : b)

struct Vetor {
    float *array;
    int size;
};

struct Node {
    float valor;
    int linha;
    int coluna;
    Node *nextRight;
    Node *nextDown;
};

struct LinkedList {
    Node *head;
    Node *tail;
    int size;
};

struct Matriz {
    LinkedList **linhas;
    LinkedList **colunas;
    int qtdLinhas;
    int qtdColunas;
    int qtd_nnz;
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

/* ============ NODE FUNCTIONS ============ */

Node *_node_construct(int linha, int coluna, float valor, Node *right, Node *down)
{
    Node *n = (Node *)malloc(sizeof(Node));

    n->valor = valor;
    n->linha = linha;
    n->coluna = coluna;
    n->nextRight = right;
    n->nextDown = down;

    return n;
}

void _node_destroy(Node *n)
{
    free(n);
}

/* ============ LINKED LIST FUNCTIONS ============ */

LinkedList *_linked_list_construct()
{
    LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;

    return list;
}

void _linked_list_destroy_lin(LinkedList *list)
{
    Node *n = list->head, *prev = NULL;

    while (n != NULL)
    {
        prev = n;
        n = n->nextRight;

        _node_destroy(prev);
    }

    free(list);
}

void _linked_list_destroy_col(LinkedList *list)
{
    free(list);
}

/* ============ MATRIZ FUNCTIONS ============ */

Matriz *matriz_construct(int qtdLinhas, int qtdColunas, int qtd_nnz)
{
    Matriz *m = (Matriz *)malloc(sizeof(Matriz));

    m->linhas = (LinkedList **)malloc(sizeof(LinkedList *) * qtdLinhas);
    m->colunas = (LinkedList **)malloc(sizeof(LinkedList *) * qtdColunas);

    for (int i = 0; i < qtdLinhas; i++)
        m->linhas[i] = _linked_list_construct();
    for (int i = 0; i < qtdColunas; i++)
        m->colunas[i] = _linked_list_construct();
    
    m->qtdLinhas = qtdLinhas;
    m->qtdColunas = qtdColunas;
    m->qtd_nnz = qtd_nnz;

    return m;
}

Vetor *matriz_multiply_by_vector(Matriz *m, Vetor *v)
{
    if (m->qtdColunas != v->size)
    {
        printf("Erro: Operação matriz x vetor invalida!\n");
        return NULL;
    }

    Vetor *resultado = vector_construct(m->qtdLinhas);

    LinkedList *current_list;
    Node *current_node;

    for (int i = 0; i < m->qtdLinhas; i++)
    {
        current_list = m->linhas[i];

        current_node = current_list->head;

        while (current_node != NULL)
        {
            resultado->array[i] += current_node->valor * v->array[current_node->coluna-1];
            current_node = current_node->nextRight;
        }
    }

    return resultado;
}

// Essa função é para a inserção de um valor especificamente para o caso da leitura do arquivo .mtx
//
void matriz_add_value(Matriz *m, int linha, int coluna, float valor)
{
    LinkedList *lin_list = m->linhas[linha-1];
    LinkedList *col_list = m->colunas[coluna-1];

    Node *n = _node_construct(linha, coluna, valor, NULL, NULL);

    if (lin_list->tail != NULL)
        lin_list->tail->nextRight = n;
    else
        lin_list->head = n;
    
    lin_list->tail = n;
    lin_list->size++;

    if (col_list->tail != NULL)
        col_list->tail->nextDown = n;
    else
        col_list->head = n;
    
    col_list->tail = n;
    col_list->size++;
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

    Matriz *m = matriz_construct(qtdLinhas, qtdColunas, qtd_nnz);

    int lin, col;
    float valor;

    while (fread(&c, sizeof(char), 1, file) == 1) // fase 3: Ler os valores da matriz
    {
        flag = (c == '\n') ? 1 : 0;

        linha[string_size] = c;
        linha[++string_size] = '\0';

        if (flag)
        {
            sscanf(linha, "%d %d %f", &lin, &col, &valor);

            matriz_add_value(m, lin, col, valor);
            flag = 0;
            string_size = 0;
        }
    }

    fclose(file);
    
    return m;
}

// Printa a matriz andando no sentido das linhas
//
void matriz_print_esparso(Matriz *m)
{
    if (m == NULL)
        return;

    LinkedList *curr_list;
    Node *curr_node;

    for (int i = 0; i < m->qtdColunas; i++)
    {
        curr_list = m->colunas[i];

        curr_node = curr_list->head;

        while (curr_node != NULL)
        {
            printf("(%d, %d): %.4f\n", curr_node->linha, curr_node->coluna, curr_node->valor);
            curr_node = curr_node->nextDown;
        }
    }
}

void matriz_destroy(Matriz *m)
{
    if (m == NULL)
        return;

    for (int i = 0; i < m->qtdLinhas; i++)
        _linked_list_destroy_lin(m->linhas[i]);
    for (int i = 0; i < m->qtdColunas; i++)
        _linked_list_destroy_col(m->colunas[i]);


    free(m->linhas);
    free(m->colunas);
    free(m);
}

/* -------------- ILU FUNCTIONS -------------- */

/**
 * Nodes são sempre adicionados no sentido da linha
 * 
 * @param m Matriz a ser preenchida
 * 
 * @param n Node a ser adicionado
*/
void _matrix_add_node(Matriz *m, Node *n)
{

}

/**
 * Nodes são adicionados a matriz em qualquer lugar
 * 
 * @param m Matriz a ser preenchida
 * 
 * @param n Node a ser adicionado
*/
void _matrix_add_node_fill_part(Matriz *m, Node *n)
{

}

void ilup_setup(Matriz *m, Matriz *L, Matriz *U, int p)
{
    int n = m->qtdLinhas;
	int* levls = NULL;
	int* jbuf  = NULL;
	int* iw;
	int** ulvl;

	int i, j, k, col, ip, it, jpiv;
	int incl, incu, jmin, kmin;

    Node *node_i = NULL;
    Node *new_node = NULL;
  
	levls = (int*)  malloc(n*sizeof(int));
	jbuf  = (int*)  malloc(n*sizeof(int)); 
	ulvl  = (int**) malloc(n*sizeof(int*));
    iw    = (int*)  malloc(n*sizeof(int));

    /**
     * Toda vez que houver um preenchimento no vetor jbuf um novo node é criado
     * 
     */

	/* initilize iw */
	for(j = 0; j < n; j++) iw[j] = -1;

	for(i = 0; i < n; i++) 
	{
		incl = 0;
		incu = i; 
		/*-------------------- assign lof = 0 for matrix elements */
        /*-------------------- mesmo algoritmo, mas transferido para o caso da lista encadeada */
        node_i = m->linhas[i]->head;
        while (node_i != NULL) {
            col = node_i->coluna-1;

            if (col < i) {
                /*-------------------- L-part  */
                new_node = _node_construct(i+1, col+1, 0, NULL, NULL);
                _matrix_add_node(L, new_node);

				jbuf[incl]  = col;
				levls[incl] = 0;
				iw[col]     = incl++;
            }
            else if (col > i) {
                /*-------------------- U-part  */
                new_node = _node_construct(i+1, col+1, 0, NULL, NULL);
                _matrix_add_node(U, new_node);

				jbuf[incu]  = col;
				levls[incu] = 0;
				iw[col]     = incu++;
            }
        }
		/*-------------------- symbolic k,i,j Gaussian elimination  */ 
		jpiv = -1; 
		while (++jpiv < incl)
		{
			k = jbuf[jpiv] ; 
			/*-------------------- select leftmost pivot */
			kmin = k;
			jmin = jpiv; 
			for(j = jpiv + 1; j< incl; j++)
			{
				if(jbuf[j] < kmin)
				{
					kmin = jbuf[j];
					jmin = j;
				}
			}
			/*-------------------- swap  */  
			if(jmin != jpiv)
			{
				jbuf[jpiv]  = kmin; 
				jbuf[jmin]  = k; 
				iw[kmin]    = jpiv;
				iw[k]       = jmin; 
				j           = levls[jpiv] ;
				levls[jpiv] = levls[jmin];
				levls[jmin] = j;
				k           = kmin; 
			}
			/*-------------------- symbolic linear combinaiton of rows  */
            /*-------------------- mesmo algoritmo, mas transferido para o caso da lista encadeada */
            node_i = U->linhas[k]->head;
            while (node_i != NULL) {
                col = node_i->coluna-1;
                it  = ulvl[k][j] + levls[jpiv] + 1;

                if(it > p) continue;

				ip  = iw[col];
                if (ip == -1) {
                    if (col < i) {
                        new_node = _node_construct(i+1, col+1, 0, NULL, NULL);
                        _matrix_add_node_fill_part(L, new_node);

                        jbuf[incl]  = col;
                        levls[incl] = it;
                        iw[col]     = incl++;
                    } 
                    else if (col > i) {
                        new_node = _node_construct(i+1, col+1, 0, NULL, NULL);
                        _matrix_add_node_fill_part(U, new_node);

                        jbuf[incu]  = col;
                        levls[incu] = it;
                        iw[col]     = incu++;
                    }
                }
                else {
                    levls[ip] = min(levls[ip], it);
                }
            }
		}   /* end - while loop */
		/*-------------------- reset iw */
		for(j = 0; j < incl; j++) iw[jbuf[j]] = -1;
		for(j = i; j < incu; j++) iw[jbuf[j]] = -1;
		
		/*-------------------- copy U - part        */ 
		k = incu-i;
		if( k > 0 )
		{
			/*-------------------- update matrix of levels */
			ulvl[i]  = (int *) malloc(k*sizeof(int)); 
			memcpy(ulvl[i], levls+i, k*sizeof(int));
		}
	}
  
	/*-------------------- free temp space and leave --*/
	free(levls);
	free(jbuf);
	for(i = 0; i < n-1; i++)
	{
		if (U->linhas[i]->size)
			free(ulvl[i]) ; 
	}
	free(ulvl);
}

void ilup(Matriz *m, Matriz *L, Matriz *U, int p)
{
    ilup_setup(m, L, U, p);
}