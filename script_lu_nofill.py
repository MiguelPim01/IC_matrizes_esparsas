import subprocess
import pandas as pd
import numpy as np
import os

diretorio_matrizes = 'matrizes/lu/nnz/nofill'

matrizes = os.listdir(diretorio_matrizes)

lista_result_geral = []

for arq in matrizes:

    print(f"Matriz {arq} sendo utilizada.....")

    matriz = os.path.join(diretorio_matrizes, arq)
    
    comandos = [
        "CSR_matrix/main" + ' ' + matriz + ' ' + '0',
        "LinkedList_matrix/main" + ' ' + matriz + ' ' + '0'
    ]

    resultados = {"Matrizes" : arq}

    for comando in comandos:

        print(f"Rodando o tipo de armazenamento --> {comando.split()[0].split('/')[0]}")

        lista_out = []
        for _ in range(10):
            processo = subprocess.Popen(comando, stdout=subprocess.PIPE, text=True, shell=True)
            saida = processo.communicate()[0]
            saida = float(saida)
            lista_out.append(saida)
        
        print(f"Execuções terminadas {comando.split()[0].split('/')[0]} finalizado!")
        print("-----------------------------------------------------------------------")
        
        lista_out.remove(min(lista_out))
        lista_out.remove(max(lista_out))
        media = np.mean(lista_out)
        resultados[comando.split()[0].split('/')[0]] = media
    
    lista_result_geral.append(resultados)

# Criando um DataFrame do pandas com os resultados
df = pd.DataFrame(lista_result_geral)

nome_arquivo = "resultadoLuNoFill_nnz.csv"

# Salvar o DataFrame em uma planilha (por exemplo, um arquivo CSV)
df.to_csv(nome_arquivo, index=False, sep=',', float_format='%.6f')

lista_result_geral.clear()