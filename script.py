import subprocess
import pandas as pd
import numpy as np
import os

diretorio_matrizes = 'matrizes/por_ordem'
diretorio_vetores = 'vetores'

matrizes = os.listdir(diretorio_matrizes)

lista_result_geral = []

for arq in matrizes:
    matriz = ' ' + diretorio_matrizes + '/' + arq
    vetor = ' ' + diretorio_vetores + '/' + arq.replace(".mtx", ".txt")

    comandos = [
        "COO_matrix/main" + matriz + vetor, 
        "CSC_matrix/main" + matriz + vetor, 
        "CSR_matrix/main" + matriz + vetor, 
        "LinkedList_matrix/main" + matriz + vetor
    ]

    resultados = {"Matrizes" : arq}

    for comando in comandos:
        lista_out = []
        for _ in range(10):
            processo = subprocess.Popen(comando, stdout=subprocess.PIPE, text=True, shell=True)
            saida = processo.communicate()[0]
            saida = float(saida)
            lista_out.append(saida)
        
        lista_out.remove(min(lista_out))
        lista_out.remove(max(lista_out))
        media = np.average(lista_out)
        resultados[comando.split()[0].split('/')[0]] = media
    
    lista_result_geral.append(resultados)

# Criando um DataFrame do pandas com os resultados
df = pd.DataFrame(lista_result_geral)

# Salvar o DataFrame em uma planilha (por exemplo, um arquivo CSV)
df.to_csv("resultados_por_ordem.csv", index=False, sep=',', float_format='%.6f')