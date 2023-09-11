import sys
import xml.etree.ElementTree as ET
import math
import random
import numpy as np
import time
import tsplib95

def Guloso(alpha, n, custos):
    caminho = np.zeros(n, dtype=int)
    selecionado = np.zeros(n, dtype=bool)
    
    caminho[0] = 0 #caminho começa no vértice 0 
    selecionado[0] = True #marcar vértice 0 como selecionado
    
    atual = 1
    while atual < n:
        v = caminho[atual-1]
        LC = []
        for u in range(n):
            if selecionado[u] == False:
                LC.append((u,custos[v,u]))
        LC.sort(key=lambda x: x[1])
        
        min,max = LC[0][1], LC[len(LC)-1][1]
        limite = min + alpha * (max - min)        
        
        RCL = [x[0] for x in LC if x[1] <= limite]        
        u = random.choice(RCL)
        
        caminho[atual] = u
        selecionado[u] = True
        atual = atual + 1
    
    custo = Avaliar(caminho, n, custos)
    return (caminho, custo)

def BuscaLocal(solG, custoG, n, custos):    
    melhorSol, melhorCusto = solG, custoG    

    #usando a Vizinhança SWAP    
    while True:
        melhorou = False
        for i in range(1,n):
            for j in range(i+1,n):
                novaSol = melhorSol
                novaSol[i], novaSol[j] = novaSol[j], novaSol[i] 
                novoCusto = Avaliar(novaSol, n, custos)
                if novoCusto < melhorCusto:
                    melhorSol, melhorCusto = novaSol, novoCusto
                    melhorou = True
        if melhorou == False:
            return (melhorSol, melhorCusto)

def Avaliar(sol, n, custos):
    custo = 0
    for i in range(n-1):
        custo = custo + custos[sol[i],sol[i+1]]    
    custo = custo + custos[sol[n-1],sol[0]]        
    return custo

def Ler(nome_arquivo):
    problem = tsplib95.load(nome_arquivo)    
    
    vertices = list(problem.get_nodes())
    n = len(vertices)
    
    custos = np.zeros([n, n], dtype=float)

    for i in range(n):
        for j in range(n):
            edge = i,j
            custos[i][j] = problem.get_weight(*edge)

    return n, custos

def GRASP(n, custos, alpha, niters):
    melhor_sol = 0
    melhor_custo = math.inf
    start = time.process_time()
    
    for it in range(0,niters):
        solG, custoG = Guloso(alpha, n, custos)
        solBL, custoBL = BuscaLocal(solG, custoG, n, custos)
        
        if custoBL < melhor_custo:
            melhor_sol, melhor_custo = solBL, custoBL
        
        #print(f"Iteracao {it}: Custo Guloso: {custoG}, Custo Busca Local: {custoBL}, Melhor Custo até agora: {melhor_custo}")    
        print(f"{alpha}\t{custoG}\t{custoBL}")    

    end = time.process_time()
    print(f"Fim\t{alpha}\t{melhor_custo}\t{end - start}")
    return (melhor_sol, melhor_custo)    


if __name__ == '__main__':
    if len(sys.argv) <= 3:
        print("Erro nos parâmetros")
    else:
        n, custos = Ler(sys.argv[1])
        #(sol, custo) = GRASP(n, custos, float(sys.argv[2]), int(sys.argv[3]))
        alpha = [0, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]
        for a in alpha:
            (sol, custo) = GRASP(n, custos, a, 1000)
        