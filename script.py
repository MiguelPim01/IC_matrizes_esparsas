import subprocess
import pandas as pd

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
    resultados[comando] = []
    for _ in range(10):
        processo = subprocess.Popen(comando, stdout=subprocess.PIPE, text=True, shell=True)
        saida = processo.communicate()[0]
        resultados[comando].append(saida)


lista_resultados = []

for i in range(10):
    linha_resultado = {"Execução": i+1}
    for comando, saidas in resultados.items():
        linha_resultado[comando.split()[0].split('/')[0]] = saidas[i]
    lista_resultados.append(linha_resultado)

# Criando um DataFrame do pandas com os resultados
df = pd.DataFrame(lista_resultados)

# Salvar o DataFrame em uma planilha (por exemplo, um arquivo CSV)
df.to_csv("resultados.csv", index=False)