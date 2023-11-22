'''
--------------------IMPORTANTE--------------------

Entrada: um número natural (0 incluso, 0 é natural!!!!) lido pela entrada padrão.

O teste de corretude é feito de maneira similar ao lab8.

Uma passagem com o algoritmo sequencial é feita e cada primo
encontrado é passado para a variável primosEncontrados.

Em seguida, uma passagem concorrente é feita com os processos
da pool de processos. A cada primo encontrado por um processo
é feita uma checagem no dicionário "populado" pela versão
sequencial. Caso algum processo determine um número primo que
não foi considerado pela versão sequencial, um erro é disparado
e o programa encerrado. Esse caso indicaria que o código concorrente
não está correto.

Obs: o ganho começa a "valer a pena" com n = 310000. O output foi esse:

tempo levado pelo sequencial: 0.5781099796295166
tempo levado pelo concorrente com 12 processos: 0.5781264305114746

"valer a pena" aqui é só um modo de dizer que os tempos se aproximam.
A partir "aproximadamente" desse ponto, o concorrente começa a ganhar
em tempo em comparativo ao sequencial.

output com n = 310100:

tempo levado pelo sequencial: 0.5937478542327881
tempo levado pelo concorrente com 12 processos: 0.5781266689300537

--------------------------------------------------
'''
from multiprocessing.pool import Pool
import time
import sys
import math
import os

#variável global que marca os primos encontrados
primosEncontrados = dict()

#task para os processos da pool de processos
def task(n):
    if(ehPrimo(n) and primosEncontrados[n]):
        return n
    else:
        reportaErro("divergencia")

#função para verificar primalidade
def ehPrimo(n):
    if (n <= 1): return False
    if (n == 2): return True
    if (n % 2 == 0): return False

    i = 3
    lim = math.sqrt(n)+1

    while(i < lim):
        if(n % i == 0): return False
        i += 2
    return True

#função para reportar possíveis erros
def reportaErro(e, n = 0):
    if e == "divergencia":
        print("divergência entre sequencial e concorrente em: " + n)
        print("Tal número não foi considerado primo na versão sequencial!")
    elif e == "argInsuf":
        print("ERRO: digite até que número quer checar primalidade")
    elif e == "argInv":
        print("ERRO: número inválido")
    else:
        print("ERRO: o erro que seria reportado não é conhecido")
    sys.exit()


#main
if __name__ == "__main__":

    #trata argumentos da entrada padrão
    n = 0
    if len(sys.argv) < 2:
        reportaErro("argInsuf")
    elif int(sys.argv[1]) < 0:
        reportaErro("argInv")
    else:
        n = int(sys.argv[1])

    #definição do intervalo para checagem de primalidade
    numeros = list(range(1, n+1))

    #sequencial
    start = time.time()
    for i in numeros:
        if ehPrimo(i):
            primosEncontrados[i] = True
    end = time.time()
    print("tempo levado pelo sequencial: {}".format(end - start))
    
    #concorrente
    start = time.time()
    #cria uma pool de n processos sendo n = numero de processadores da máquina
    pool = Pool()
    resultados = pool.map(ehPrimo, numeros)
    #print(resultados)
    end = time.time()
    print("tempo levado pelo concorrente com {} processos: {}".format(pool._processes, (end - start)))