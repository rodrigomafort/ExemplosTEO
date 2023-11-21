#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <string>

using namespace std;

// Função para realizar a leitura dos dados a partir do arquivo
void lerDadosArquivo(const char *nomeArquivo, int &n, int &c, int &z, float &tempo, vector<int> &p, vector<int> &w, vector<int> &x) 
{
    p.clear();
    w.clear();
    x.clear();

    // Abrir o arquivo
    std::ifstream arquivo(nomeArquivo);

    // Verificar se o arquivo foi aberto com sucesso
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo." << endl;
        return; // Sair da função em caso de erro
    }

    // Ler informações da instância
    string chave;
    arquivo >> chave >> n;
    arquivo >> chave >> c;
    arquivo >> chave >> z;
    arquivo >> chave >> tempo;

    // Ler os valores do arquivo e armazenar nos vetores
    int indice, lucro, peso, sol_x;
    char separador; 

    while (arquivo >> indice >> separador >> lucro >> separador >> peso >> separador >> sol_x)
    {
        p.push_back(lucro);
        w.push_back(peso);
        x.push_back(sol_x);
    }

    // Fechar o arquivo
    arquivo.close();
}

pair<vector<bool>, int> GerarMutante(int &n, int &c, vector<int> &p, vector<int> &w, mt19937 gen)
{
    vector<bool> novo(n, false);
    int peso = 0;
    int lucro = 0;
    discrete_distribution<int> distPesos(p.begin(), p.end());

    while (peso < c)
    {
        int e;
        do
        {
            e = distPesos(gen);
        } 
        while (novo[e]);

        novo[e] = true;
        peso += w[e];
        lucro += p[e];
    }

    return make_pair(novo, lucro);
}

void AplicarCrossOver(int &n, vector<bool> &paiElite, vector<bool> &paiComum, vector<bool> &filho, float alpha, mt19937 gen)
{
    uniform_real_distribution<> dis(0.0, 1.0);
    for (int i = 0; i < n; i++)
    {
        if (dis(gen) <= alpha)
            filho[i] = paiElite[i];
        else
            filho[i] = paiComum[i];
    }
}

void AplicarMutacao(int &n, vector<bool> &filho, float beta, mt19937 gen)
{
    uniform_real_distribution<> dis(0.0, 1.0);
    for (int i = 0; i < n; i++)
    {
        if (dis(gen) <= beta)
            filho[i] = filho[i] == true? false : true;
    }
}

pair<vector<bool>, int> GerarIndividuo(vector<bool> &paiElite, vector<bool> &paiComum, float alpha, float beta, int &n, int &c, vector<int> &p, vector<int> &w, mt19937 gen)
{
    vector<bool> novo(n, false);
    AplicarCrossOver(n, paiElite, paiComum, novo, alpha, gen);
    AplicarMutacao(n, novo, beta, gen);

    int lucro = 0;
    int peso = 0;
    
    
    do
    {    
        lucro = 0;
        peso = 0;

        vector<int> sol;
    
        for (int i = 0; i < n; i++)
        {
            if (novo[i] == true)
            {
                lucro += p[i];
                peso += w[i];
                sol.push_back(i);
            }
        }    

        if (peso > c)
        {
            uniform_int_distribution<> dis(0, sol.size() - 1);
            int rem = sol[dis(gen)];
            novo[rem] = false;
        }

        //cout << lucro << " - " << peso << endl;

    } 
    while (peso > c);

    

    return make_pair(novo, lucro);
}

pair<vector<bool>,int> GA(int &n, int &c, vector<int> &p, vector<int> &w, int tamPop, float pMutante, float pElite, float alpha, float beta, int nGeracoesSMelhora)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    int nMutantes = floor(tamPop * pMutante); 
    int nElite = floor(tamPop * pElite); 
    int nReproducao = tamPop -  (nMutantes + nElite);
    
    pair<vector<bool>,int> bestGlobal;
    bestGlobal.second = -1;    
    
    vector<pair<vector<bool>,int>> genAnterior;
    vector<pair<vector<bool>,int>> genNova; 

    int ctG = 0;
    int ctGSM = 0;
    while (ctGSM <= nGeracoesSMelhora)
    {
        //cout << "GERACAO: " << ctG << " - " << genAnterior.size() << endl;
        if (ctG == 0)
        {
            //Gerar 100% de Mutantes
            for (int i = 0; i < tamPop; i++)
                genNova.push_back(GerarMutante(n, c, p, w, gen));
        }
        else
        {
            //Gerar Mutantes
            for (int i = 0; i < nMutantes; i++)
                genNova.push_back(GerarMutante(n, c, p, w, gen));

            //Copiar sobreviventes
            for (int i = 0; i < nElite; i++)        
                genNova.push_back(genAnterior[i]);

            //Gerar por crossover uniforme
            vector<double> probElite;
            vector<double> probGeral;
            
            for (int i = 0; i < tamPop; i++)
                probGeral.push_back(genAnterior[i].second);

            for (int i = 0; i < nElite; i++)
                probElite.push_back(genAnterior[i].second);

            std::discrete_distribution<int> distGeral(probGeral.begin(), probGeral.end());
            std::discrete_distribution<int> distElite(probElite.begin(), probElite.end());
            
            //Gerar por reprodução
            for (int i = 0; i < nReproducao; i++)
            {
                int paiElite = distElite(gen);
                int paiComum;
                do
                {
                    paiComum = distGeral(gen);
                } 
                while (paiElite == paiComum);

                vector<bool> paiE = genAnterior[paiElite].first;
                vector<bool> paiC = genAnterior[paiComum].first;
                
                genNova.push_back(GerarIndividuo(paiE, paiC, alpha, beta, n, c, p, w, gen));
            }
        }

        //Ordenar a nova geração de acordo com a qualidade dos indivíduos
        sort(genNova.begin(), genNova.end(), [](const std::pair<vector<bool>,int> &left, const std::pair<vector<bool>,int> &right) {  return left.second > right.second; });

        //Selecionar o melhor indivíduo
        pair<vector<bool>, int> bestNova = genNova[0];
        if (bestGlobal.second == -1 || bestGlobal.second < bestNova.second)
        {
            //cout << "NOVO BEST " << bestNova.second << endl;
            bestGlobal = bestNova;
            ctGSM = 0;
        }
        else
        {
            ctGSM++;
        }
        
        //Substituir a geração anterior
        genAnterior.clear();
        genAnterior.swap(genNova);

        ctG++;
    }

    return bestGlobal;
}

void ImprimirInstancia(int &n, int &c, int &z, float &tempo, vector<int> &p, vector<int> &w, vector<int> &x)
{
    // Exibir informações da instância
    std::cout << "n: " << n << std::endl;
    std::cout << "c: " << c << std::endl;
    std::cout << "z: " << z << std::endl;
    std::cout << "tempo: " << tempo << std::endl;

    // Exibir os vetores resultantes
    std::cout << "Vetor p: ";
    for (int i : p) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "Vetor w: ";
    for (int i : w) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "Vetor x: ";
    for (int i : x) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    int lucroX = 0;
    int pesoX = 0;
    for (int i = 0; i < n; i++)
    {
        if (x[i] == 1)
        {
            lucroX += p[i];
            pesoX += w[i];
        }
    }
    cout << "LUCRO: " << lucroX << " - PESO: " << pesoX << endl;
}

int main(int argc, char *argv[]) 
{    
    if (argc != 8) 
    {
        cerr << "Erro nos parâmetros" << endl;
        return -1;
    }
    
    
    // Variáveis para armazenar informações da instância
    int n, c, z;
    float tempo;

    // Vetores para armazenar os valores lidos do arquivo
    std::vector<int> p, w, x;

    // Chamar a função para ler os dados do arquivo
    lerDadosArquivo(argv[1], n, c, z, tempo, p, w, x);
    //ImprimirInstancia(n, c, z, tempo, p, w, x);

    int tamPop = stoi(argv[2]);
    float pMutante = stof(argv[3]);
    float pElite = stof(argv[4]);
    float alpha = stof(argv[5]);
    float beta = stof(argv[6]);
    int nIteSM = stoi(argv[7]);

    /*
    cout << "tamPop " << tamPop << endl;
    cout << "pMutante " << pMutante << endl;
    cout << "pElite " << pElite << endl;
    cout << "alpha " << alpha << endl;
    cout << "beta " << beta << endl;
    cout << "nIteSM " << nIteSM << endl;
    */
    
    pair<vector<bool>,int> sol = GA(n, c, p, w, tamPop, pMutante, pElite, alpha, beta, nIteSM);

    int lucro = 0;
    int peso = 0;
    for (int i = 0; i < n; i++)
    {
        if (sol.first[i] == true)
        {
            lucro += p[i];
            peso += w[i];
        }
    }
    cout << lucro << endl;


    return 0; // Sair do programa com código de sucesso
}
