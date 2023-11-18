import subprocess
import pandas as pd
import numpy as np

matriz = " matrizes/e40r5000.mtx"
vetor = " vetores/e40r5000.txt"

comandos = [
    "COO_matrix/main" + matriz + vetor, 
    "CSC_matrix/main" + matriz + vetor, 
    "CSR_matrix/main" + matriz + vetor, 
    "LinkedList_matrix/main" + matriz + vetor
]

resultados = {}

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
    resultados[comando] = media


lista_resultados = []


linha_resultado = {}
for comando, saida in resultados.items():
    linha_resultado[comando.split()[0].split('/')[0]] = saida
lista_resultados.append(linha_resultado)

# Criando um DataFrame do pandas com os resultados
df = pd.DataFrame(lista_resultados)

# Salvar o DataFrame em uma planilha (por exemplo, um arquivo CSV)
df.to_csv("resultados.csv", index=False, sep=',', float_format='%.6f')