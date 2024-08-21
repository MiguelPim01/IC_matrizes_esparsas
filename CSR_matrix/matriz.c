#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matriz.h"

#define min(a, b) ((a < b) ? a : b)

/**
 * COMENTÁRIOS: 
 * 
 * -> No array "ptr_coluna" se o valor for -1 significa que não existem valores naquela coluna.
 * 
 * -> Todos os valores devem ser adicionados seguindo a ordem da primeira ate a ultima coluna.
 * 
 */

struct Vetor {
    float *array;
    int size;
};

struct Valor {
    float valor;
    int coluna;
};

struct Matriz {
    Valor *valores;
    int *ptr_linha;
    int qtd_nnz, qtdLinhas, qtdColunas;
    int size;
};

/* ============ ILU FUNCTIONS ============ */

/*----------------------------------------------------------------------------
 * Prepare space of a row according to the result of level structure
 *--------------------------------------------------------------------------*/
void SPARILU_row (SparILU* lu, int nrow)
{
	int nzcount;
	nzcount = lu->L->nzcount[nrow];
	if (nzcount) {
		free(lu->L->ma[nrow]);
		lu->L->ma[nrow] = calloc(nzcount,sizeof(float));
	}
	nzcount = lu->U->nzcount[nrow];
	if (nzcount) {
		free(lu->U->ma[nrow]);
		lu->U->ma[nrow] = calloc(nzcount,sizeof(float));
	}
}

/*----------------------------------------------------------------------------
 * Initialize SparMAT structs
 *--------------------------------------------------------------------------*/
void SparMAT_setup(SparMAT *mat, int n)
{
    mat->n       = n;
    if ((mat->nzcount = (int *)    calloc(n,sizeof(int))) == NULL)       { printf ("ERRO ALOCACAO of mat->nzcount!\n"); exit(1); }
	if ((mat->ja      = (int **)   calloc(n,sizeof(int*))) == NULL)      { printf ("ERRO ALOCACAO of mat->ja\n"); exit(1); }
	if ((mat->ma      = (float **) calloc(n,sizeof(float*))) == NULL)    { printf ("ERRO ALOCACAO of mat->ma\n"); exit(1); }
}

/*----------------------------------------------------------------------------
 * Initialize SparILU structs
 *--------------------------------------------------------------------------*/
void SPARILU_setup (SparILU* lu, int n)
{
	lu->n    = n;
	lu->D    = calloc(n, sizeof(double));

	lu->L = (SparMAT *)malloc(sizeof(SparMAT));
	SparMAT_setup(lu->L, n);
	lu->U = (SparMAT *)malloc(sizeof(SparMAT));
	SparMAT_setup(lu->U, n);

	lu->work = calloc(n, sizeof(int));
}

/*----------------------------------------------------------------------------
 * Initialize CSR structs
 *--------------------------------------------------------------------------*/
void CSR_setup(Matriz *csr, int n, int nzcount)
{
	csr->ptr_linha  = (int *)   calloc(n + 1, sizeof(int));
	csr->valores    = (Valor *) calloc(nzcount + n, sizeof(Valor));

	csr->qtd_nnz    = nzcount;
	csr->qtdLinhas  = n;
	csr->qtdColunas = n;
	csr->size       = 0;
}

/*----------------------------------------------------------------------------
 * Transforms CSR struct to SparMAT struct
 *--------------------------------------------------------------------------*/
void CSR_to_SparMAT(Matriz *csr, SparMAT *mat)
{
    int n = csr->qtdLinhas;
    int tam_lin, prox_lin;
    int i, j, k;

    /*------------------------------------- SparMAT setup */
    SparMAT_setup(mat, n);
    /*--------------------------------------------------- */

    for (i = 0; i < n; i++) {

		j = i+1;
		while (csr->ptr_linha[j] < 0) {
			j++;
		}
		i = j-1;

        tam_lin = csr->ptr_linha[i+1] - csr->ptr_linha[i];

        mat->nzcount[i] = tam_lin;
        mat->ja[i]      = (int *)   malloc(tam_lin * sizeof(int));
        mat->ma[i]      = (float *) malloc(tam_lin * sizeof(float));

        for (j = csr->ptr_linha[i], k = 0; j < csr->ptr_linha[i+1]; j++, k++) {
            mat->ja[i][k] = csr->valores[j].coluna-1;
            mat->ma[i][k] = csr->valores[j].valor;
        }
    }
}

/*----------------------------------------------------------------------------
 * Transforms SparMAT from SparILU struct to CSR struct
 *--------------------------------------------------------------------------*/
void SparMAT_to_CSR(SparMAT *mat, Matriz *csr, float *D, char c)
{
	int n = mat->n;
	int i, k, j;
	int qtd_nnz = 0;

	for (i = 0; i < n; i++) { qtd_nnz += mat->nzcount[i]; }

	CSR_setup(csr, n, qtd_nnz);

	qtd_nnz = 0;

	if (c == 'L') {

		for (i = 0; i < n; i++) {

			csr->ptr_linha[i] = qtd_nnz;

			for (k = 0, j = qtd_nnz; k < mat->nzcount[i]; k++, j++) {

				csr->valores[j].coluna = mat->ja[i][k]+1;
				csr->valores[j].valor  = mat->ma[i][k];
			}

			qtd_nnz += mat->nzcount[i]+1;

			// Armazena a diagonal da matriz
			csr->valores[j].coluna = i+1;
			csr->valores[j].valor = 1;
			
		}
	}
	else if (c == 'U') {

		for (i = 0; i < n; i++) {

			csr->ptr_linha[i] = qtd_nnz;
			
			// Armazena a diagonal da matriz
			csr->valores[qtd_nnz].coluna = i+1;
			csr->valores[qtd_nnz].valor = 1/D[i];

			for (k = 0, j = qtd_nnz+1; k < mat->nzcount[i]; k++, j++) {

				csr->valores[j].coluna = mat->ja[i][k]+1;
				csr->valores[j].valor  = mat->ma[i][k];
			}

			qtd_nnz += mat->nzcount[i]+1;
		}
	}

	csr->ptr_linha[i] = qtd_nnz;
}

void SparILU_to_CSR(SparILU *lu, Matriz *L, Matriz *U)
{
    SparMAT_to_CSR(lu->L, L, lu->D, 'L');
	SparMAT_to_CSR(lu->U, U, lu->D, 'U');
}

void SparMAT_destroy(SparMAT *mat)
{
	for (int i = 0; i < mat->n; i++) {
		if (mat->nzcount[i]) {
			free(mat->ja[i]);
			free(mat->ma[i]);
		}
	}

	free(mat->ja);
	free(mat->ma);
	free(mat->nzcount);
	free(mat);
}

void SparILU_destroy(SparILU *lu)
{
	SparMAT_destroy(lu->L);
	SparMAT_destroy(lu->U);
	free(lu->D);
	free(lu->work);
	free(lu);
}

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

/* ============ VALOR FUNCTIONS ============ */

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

/* ============ MATRIZ FUNCTIONS ============ */

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

Matriz *matriz_build()
{
	return (Matriz *)malloc(sizeof(Matriz));
}

void matriz_add_value(Matriz *m, Valor v, int linha)
{
    int ind_linha = m->ptr_linha[linha-1];

    m->ptr_linha[linha-1] = (ind_linha == -1) ? m->size : ind_linha;

    m->valores[m->size] = v;
    m->size++;
}

Vetor *matriz_multiply_by_vector(Matriz *m, Vetor *v)
{
    if (m->qtdColunas != v->size)
        return NULL;
    
    Vetor *resultado = vector_construct(m->qtdLinhas);
    int cond;

    for (int i = 0; i < m->qtdLinhas; i++)
    {
        cond = m->ptr_linha[i+1]; // variavel adicionada para diminuir o numero de acessos a struct e array

        for (int k = m->ptr_linha[i]; k < cond; k++)
        {
            resultado->array[i] += v->array[m->valores[k].coluna-1] * m->valores[k].valor;
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

    while (fread(&c, sizeof(char), 1, file) == 1) // fase 3: Ler os valores da matriz
    {
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

void matriz_print_esparso(Matriz *m)
{
    if (m == NULL)
        return;
    
    for (int i = 0; i < m->qtdLinhas; i++)
        for (int k = m->ptr_linha[i]; k < m->ptr_linha[i+1]; k++)
            printf("(%d, %d): %.4f\n", i+1, m->valores[k].coluna, m->valores[k].valor);
}

void matriz_destroy(Matriz *m)
{
    if (m == NULL)
        return;

    free(m->ptr_linha);
    free(m->valores);
    free(m);
}

int matriz_get_ordem(Matriz *m)
{
	return m->qtdLinhas;
}

/* ============ ILU ============ */

/**
 * ilu_setup faz as previsões de preenchimentos para preparar a estrutura da matriz L e U a recebê-los.
 *
 * Nessa função não é necessário utilizar os nnz da matriz.
 * 
 * @param m matriz que será fatorizada em L e U
 * 
 * @param L matriz triangular inferior da fatoração
 * 
 * @param U matriz triangular superior da fatoração
 * 
 * @param p level de fill in
 */
void ilup_setup(SparMAT *m, SparILU *lu, int p)
{
    int n = m->n;
	int* levls = NULL;
	int* jbuf  = NULL;
	int* iw    = lu->work;
	int** ulvl;  /*  stores lev-fils for U part of ILU factorization*/
	SparMAT* L = lu->L; 
	SparMAT* U = lu->U;
	/*--------------------------------------------------------------------
	* n        = number of rows or columns in matrix
	* inc      = integer, count of nonzero(fillin) element of each row
	*            after symbolic factorization
	* ju       = entry of U part of each row
	* lvl      = buffer to store levels of each row
	* jbuf     = buffer to store column index of each row
	* iw       = work array
	*------------------------------------------------------------------*/
	int i, j, k, col, ip, it, jpiv;
	int incl, incu, jmin, kmin;

	int qtd_preenchimento = 0;
  
	levls = (int*)  malloc(n*sizeof(int));
	jbuf  = (int*)  malloc(n*sizeof(int)); 
	ulvl  = (int**) malloc(n*sizeof(int*));

	/* initilize iw */
	// ===================================================================================
	// ANÁLISE DE COMPLEXIDADE DA LINHA 499 ATÉ 524
	//
	/**
	 * Linha 519:
	 *     - {n+1} operações (=), {n+1} comparações, {n} incrementos, {n} acessos a array
	 * 
	 * Linha 520:
	 *     - {1} operação (=), {n+1} comparações, {n} incrementos
	 * 
	 * Linha 522 e 523:
	 *     - {2n} operações (=)
	 * 
	 * Linha 525:
	 *     - {n} operações (=), {nnz+n} comparações, {nnz} incrementos, {nnz+n} acessos a array
	 * 
	 * Linha 527:
	 *     - {nnz} operações (=), {2nnz} acessos a array, {nnz} acessos a memória (pointer)
	 * 
	 * Linha 528 até 544:
	 *     - {3nnz + n} operações (=), {2nnz} comparações, {nnz} incrementos, {3nnz} acessos a array
	 */
	for(j = 0; j < n; j++) iw[j] = -1;
	for(i = 0; i < n; i++) 
	{
		incl = 0;
		incu = i; 
		/*-------------------- assign lof = 0 for matrix elements */
		for(j = 0; j < m->nzcount[i]; j++) 
		{
			col = m->ja[i][j];
			if(col < i) 
			{
				/*-------------------- L-part  */
				jbuf[incl]  = col;
				levls[incl] = 0;
				iw[col]     = incl++;
			} 
			else if (col > i) 
			{ 
				/*-------------------- U-part  */
				jbuf[incu]  = col;
				levls[incu] = 0;
				iw[col]     = incu++;
			} 
		}
		/*-------------------- symbolic k,i,j Gaussian elimination  */ 
		jpiv = -1;
	/**
	 * Total:
	 *     - operações         = (5n + 4nnz + 2)
	 *     - comparações       < (3n + 3nnz + 2)
	 *     - incrementos       = (2n + 2nnz)
	 *     - acessos a array   = (2n + 6nnz)
	 *     - acessos a memória = nnz
	 */
	// ===================================================================================
	// ANÁLISE DE COMPLEXIDADE DA LINHA 564 ATÉ 589
	//
	/**
	 * Linha 567 até 572:
	 *     - {3(nnz-n)/2} operações (=), {(nnz+n)/2} comparações, {(nnz+n)/2} incrementos, {(nnz-n)/2} acessos a array
	 * 
	 * Linha 573 até 580:
	 *     - {} operações (=, +), {} comparações, {} incrementos, {} acessos a array --- FALTANDO FAZER
	 * 
	 * Linha 582 até 592:
	 *     - {4(nnz-n)} operações (=), {(nnz-n)/2} comparações, {0} incrementos, {4(nnz-n)} acessos a array
	 * 
	*/
		while (++jpiv < incl)
		{
			k = jbuf[jpiv] ; 
			/*-------------------- select leftmost pivot */
			kmin = k;
			jmin = jpiv; 
			for(j = jpiv + 1; j < incl; j++)
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

	/**
	 * Total:
	 *     - operações         = (5nnz/2 - 5n/2)
	 *     - comparações       = (nnz)
	 *     - incrementos       = (nnz/2 + n/2)
	 *     - acessos a array   = (9nnz/2 - 9n/2)
	 *     - acessos a memória = (0)
	*/
	// ===================================================================================
	// ANÁLISE DE COMPLEXIDADE DA LINHA 619 ATÉ 664 
	//
	/**
	 * Linha 619 até 624:
	 *     - {} operações (=, +), {} comparações, {} incrementos, {} acessos a array
	 * 
	 * Linha 625 até 642:
	 *     - {} operações (=, +), {} comparações, {} incrementos, {} acessos a array
	 * 
	 * Linha 646 até 664:
	 *     - {15n + 2nnz} operações (=, -, *, +), {4n + 2nnz} comparações, {2n + 2nnz} incrementos, {8n + 4nnz} acessos a array, {6n} acessos a memória
	 * 
	 * 
	 */

			/*-------------------- symbolic linear combinaiton of rows  */
			for(j = 0; j < U->nzcount[k]; j++)
			{
				col = U->ja[k][j];
				it  = ulvl[k][j]+levls[jpiv]+1 ; 
				if(it > p) continue; 
				ip  = iw[col];
				if( ip == -1 )
				{
					qtd_preenchimento++;
					if(col < i)
					{
						jbuf[incl]  = col;
						levls[incl] = it;
						iw[col]     = incl++;
					} 
					else if(col > i)
					{
						jbuf[incu]  = col;
						levls[incu] = it;
						iw[col]     = incu++;
					} 
				}
				else
					levls[ip] = min(levls[ip], it); 
			}
		}   /* end - while loop */
		/*-------------------- reset iw */
		for(j = 0; j < incl; j++) iw[jbuf[j]] = -1;
		for(j = i; j < incu; j++) iw[jbuf[j]] = -1;
		/*-------------------- copy L-part */ 
		L->nzcount[i] = incl;
		if(incl > 0 )
		{
			L->ja[i] = (int *) malloc(incl*sizeof(int));
			memcpy(L->ja[i], jbuf, incl*sizeof(int));
		}
		/*-------------------- copy U - part        */ 
		k = incu-i; 
		U->nzcount[i] = k; 
		if( k > 0 )
		{
			U->ja[i] = (int *) malloc(k*sizeof(int));
			memcpy(U->ja[i], jbuf+i, k*sizeof(int));
			/*-------------------- update matrix of levels */
			ulvl[i]  = (int *) malloc(k*sizeof(int)); 
			memcpy(ulvl[i], levls+i, k*sizeof(int));
		}
	/**
	 * Total:
	 *     - operações         = ()
	 *     - comparações       = ()
	 *     - incrementos       = ()
	 *     - acessos a array   = ()
	 *     - acessos a memória = ()
	*/
	// ===================================================================================
	}
	// ANÁLISE DE COMPLEXIDADE DAS LINHAS 685 ATÉ 692
	//
	/**
	 * Linha 685 até 692:
	 *     - {(n+1)} operações (=, -), {n} comparações, {n} incrementos, {(n-1)} acessos a array, {(n-1)} acessos a memória
	 * 
	 */
  
	/*-------------------- free temp space and leave --*/
	free(levls);
	free(jbuf);
	for(i = 0; i < n-1; i++)
	{
		if (U->nzcount[i])
			free(ulvl[i]) ; 
	}
	free(ulvl);

	/**
	 * Total GERAL:
	 *     - operações         = ()
	 *     - comparações       = ()
	 *     - incrementos       = ()
	 *     - acessos a array   = ()
	 *     - acessos a memória = ()
	*/
	// ===================================================================================

	// printf("QUANTIDADE DE PREENCHIMENTO: %d\n", qtd_preenchimento);

	return;
}

void ilup(SparMAT *m, SparILU *lu, int p)
{
	ilup_setup(m, lu, p);

    int n = m->n;
	int* jw, i, j, k, col, jpos, jrow;
	SparMAT* L;
	SparMAT* U;
	float*  D;
	
	L = lu->L;
	U = lu->U;
	D = lu->D;

	jw = lu->work;
	/* set indicator array jw to -1 */
	for(j = 0; j < n; j++) jw[j] = -1;

	/* beginning of main loop */
	for(i = 0; i < n; i++)
	{
		/* set up the i-th row accroding to the nonzero
		 * information from symbolic factorization */
		SPARILU_row (lu, i);

		/* setup array jw[], and initial i-th row */
		
		for(j = 0; j < L->nzcount[i]; j++)
		{  /* initialize L part   */
			col         = L->ja[i][j];
			jw[col]     = j;
			L->ma[i][j] = 0;
		}
		
		jw[i] = i;		
		D[i]  = 0; /* initialize diagonal */
		
		for(j = 0; j < U->nzcount[i]; j++)
		{  /* initialize U part   */
			col         = U->ja[i][j];
			jw[col]     = j;
			U->ma[i][j] = 0;
		}
		/* copy row from m into lu */
		for(j = 0; j < m->nzcount[i]; j++)
		{
			col  = m->ja[i][j];     
			jpos = jw[col];
			if(col < i)
				L->ma[i][jpos] = m->ma[i][j];
			else if(col == i)
				D[i] = m->ma[i][j];
			else
				U->ma[i][jpos] = m->ma[i][j];
		} 
		/* eliminate previous rows */
		for(j = 0; j < L->nzcount[i]; j++)
		{
			jrow = L->ja[i][j];
			/* get the multiplier for row to be eliminated (jrow) */
			L->ma[i][j] *= D[jrow];

			/* combine current row and row jrow */
			for(k = 0; k < U->nzcount[jrow]; k++)
			{
				col  = U->ja[jrow][k];
				jpos = jw[col];
				if(jpos == -1) continue;
				if(col < i)
					L->ma[i][jpos] -= L->ma[i][j] * U->ma[jrow][k];
				else if(col == i)
					D[i] -= L->ma[i][j] * U->ma[jrow][k];
				else
					U->ma[i][jpos] -= L->ma[i][j] * U->ma[jrow][k];
			}
		}

		/* reset double-pointer to -1 ( U-part) */
		for(j = 0; j < L->nzcount[i]; j++)
		{
			col     = L->ja[i][j];
			jw[col] = -1;
		}
		jw[i] = -1;
		for(j = 0; j < U->nzcount[i]; j++)
		{
			col     = U->ja[i][j];
			jw[col] = -1;
		}

		if(D[i] == 0)
		{
			for(j = i+1; j < n; j++)
			{
				L->ma[j] = NULL;
				U->ma[j] = NULL;
			}
			printf( "fatal error: Zero diagonal found...\n" );
			exit(1);
		}
		D[i] = 1.0 / D[i];
	}
	return;
}