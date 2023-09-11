#!/usr/bin/python

import sys
from matplotlib.colors import Colormap
import numpy as np
import pandas as pd
import matplotlib
import matplotlib.pyplot as plt
import locale
from matplotlib.ticker import (MultipleLocator, AutoMinorLocator)
from scipy.interpolate import interp1d
import math

def ImportarDados():	
    alpha = [0, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]
    linhas = dict()
    for a in alpha:
        linhas[a] = ([], [])

        
    f = open('br.txt','r')
    for line in f.readlines():        
        l = [x.strip() for x in line.strip().split('\t')]
        if len(l) == 3:
            a = float(l[0])
            gl = float(l[1])
            bl = float(l[2])
            linhas[a][0].append(gl)
            linhas[a][1].append(bl)
        else:
            a = float(l[1])
            sol = float(l[2])
            tempo = float(l[3])
            print(f"{a}\t{sol}\t{tempo}")
        
    
    return linhas
    
def GerarGrafico(dados):

    min = math.inf
    max = -math.inf

    alpha = [0, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]
    for a in alpha:
        for x in dados[a][0]:
            if x < min: min = x
            if x > max: max = x
        for x in dados[a][1]:
            if x < min: min = x
            if x > max: max = x
    
    soma = 0

    for a in alpha:
    #a = alpha[1]
        fig, ax = plt.subplots()
        ax.hist(dados[a], alpha=0.5, label=['Guloso', 'Busca Local'], histtype='bar', ec='black')
        ax.set_xlim(min,max)                    
        plt.title(f'Alpha = {a}')
                
        fig.tight_layout()
        fig.set_size_inches(w=12, h=6)
        plt.savefig(f"fig_{a}.png")
	    

    
	
	

dados = ImportarDados()
#GerarGrafico(dados)



