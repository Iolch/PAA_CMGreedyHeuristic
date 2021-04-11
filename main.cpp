#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <stdlib.h>
#include <fstream> 
#include <map>
#include <vector>
#include <sstream>
#include <string>
#include <chrono>


std::vector<std::string> cliqueMax;
int limitIter = 10;

// Essa função printa um vetor de string. Ex: um clique
void print(std::string text, std::vector<std::string> &vector){
    std::cout << text << std::endl;
    for (auto &element : vector){ 
            std::cout << element << " ";   
    }
    std::cout << std::endl;
}

// Essa função é responsável por preencher a matriz de vizinhos de um vértice.
void readToNeighboursMatrix(std::string filepath, std::map<std::string, std::vector<std::string>> &neighboursMatrix){
    std::cout << "Lendo arquivo: " << filepath << std::endl;

    // Lê arquivo
    std::ifstream reader(filepath);
    std::string line;
    int i = 0;
    
    // Enquanto tiver linhas
    while (getline (reader, line)) {
        i += 1;

        // Imprima o que está a partir da 2ª linha
        // Já que os dados da DIMAC são nesse formato
        if(i > 2) {

            // Vamos dar um "explode" na line, ficando somente com os nodes
            std::stringstream ssin(line);
            std::string nodes[2];
            int j = 0;
            while (ssin.good() && j < 2){
                ssin >> nodes[j];
                ++j;
            }
            // Agora vamos adicionar a informação à matriz de adjacencia
            neighboursMatrix[nodes[0]].push_back(nodes[1]);
            neighboursMatrix[nodes[1]].push_back(nodes[0]);
            
        }
        
    }

    // Feche o arquivo
    reader.close(); 
}

// Essa função irá ordenar de acordo com o grau
void modifiedCountSort(std::vector<std::string> &output, int higherDegree, std::vector<std::string> &nodes, std::map<std::string, std::vector<std::string>> &neighboursMatrix){
    int count[higherDegree+1] = {0};
    for (auto &node : nodes){
        ++ count[neighboursMatrix[node].size()];    // neighboursMatrix[node].size() é o grau do vértice
    }
    for(int i = 1; i <= higherDegree; i++){
        count[i] = count[i] + count[i-1];
    }

    for (auto &node : nodes){
        output[count[neighboursMatrix[node].size()] - 1] = node; 
        count[neighboursMatrix[node].size()] = count[neighboursMatrix[node].size()] - 1;       
    }    

}

int findHigherDegree(std::map<std::string, std::vector<std::string>> &neighboursMatrix){
    int higherDegree = 0;
    std::map<std::string, std::vector<std::string>>::iterator it;
    
    for (it = neighboursMatrix.begin(); it != neighboursMatrix.end(); it++)
    {
        if((int) it->second.size() > higherDegree) higherDegree = (int) it->second.size();
    }
    return higherDegree;
    
}

std::vector<std::string> intersection(std::vector<std::string> &v1,
                                      std::vector<std::string> &v2){
    std::vector<std::string> v3;

    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());

    std::set_intersection(v1.begin(),v1.end(),
                          v2.begin(),v2.end(),
                          back_inserter(v3));
    return v3;
}
std::vector<std::string> difference(std::vector<std::string> &v1,
                                      std::vector<std::string> &v2){
    std::vector<std::string> v3;

    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());

    std::set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(),
                        std::inserter(v3, v3.begin()));
    return v3;
}

void greedy(std::map<std::string, std::vector<std::string>> &neighboursMatrix, std::vector<std::string> nodes, int shouldStop, std::vector<std::string> clique){
    while(nodes.size() != 0 && shouldStop < limitIter){ 
        shouldStop ++;  // Condição de parada
        std::string node = nodes[0];    // seleciona o candidato
        nodes.erase(nodes.begin());     // remove o candidato do conjunto
        
        //caso a solução seja viável
        if((clique.size()+intersection(nodes, neighboursMatrix[node]).size()+1 > cliqueMax.size()) || clique.size() == 0){
            clique.push_back(node); // adiciona o elemento na solução
            std::vector<std::string> comumNeighbours = intersection(nodes, neighboursMatrix[node]);

            //caso tenha encontrado uma solução
            if((int)comumNeighbours.size() == 0){
                shouldStop = 0;
                if(clique.size() > cliqueMax.size()) cliqueMax = clique;    // retorna a solução
            }else{
                greedy(neighboursMatrix, comumNeighbours, shouldStop, clique);
            }

            clique.pop_back();
        }        
    }
}
void fillNodes(std::map<std::string, std::vector<std::string>> &neighboursMatrix, std::vector<std::string> &nodes){
    std::map<std::string, std::vector<std::string>>::iterator it;
    
    for (it = neighboursMatrix.begin(); it != neighboursMatrix.end(); it++) {
        nodes.push_back(it->first);
    }
}

int main(int argc, char * argv[]){
    // Recebendo os argumentos
    if(argc < 2){
        std::cout << "Insira o caminho de um arquivo: " << std::endl;
        return 0;
    }
    if(argc > 3){
        std::cout << "Muitos argumentos " << std::endl;
        return 0;
    }
    std::string filepath = argv[1];
    if(argv[2]) limitIter = std::stoi(argv[2]);

    // Iniciando contagem de tempo
    auto start = std::chrono::steady_clock::now();

    // Cria matriz de vizinhos   O(n)
    std::map<std::string, std::vector<std::string>> neighboursMatrix;
    readToNeighboursMatrix(filepath, neighboursMatrix);

    // Identifica grau mais alto
    int higherDegree = findHigherDegree(neighboursMatrix);

    // Agora devemos ordenar os vértices de acordo com seus graus
    std::vector<std::string> nodes; 
    fillNodes(neighboursMatrix, nodes);
    
    // Ordenando nodes por grau não crescente
    std::vector<std::string> sortedNodes = nodes; //esse é o U
    modifiedCountSort(sortedNodes, higherDegree, nodes, neighboursMatrix);
    std::reverse(sortedNodes.begin(),sortedNodes.end());

    int shouldStop = 0;
    std::vector<std::string> clique;
    greedy(neighboursMatrix, sortedNodes, shouldStop, clique);
    print("clique max", cliqueMax);

    // Para contagem de tempo
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "FINALIZA EXECUÇÃO "  << elapsed_seconds.count() << "s" << std::endl;
   

    return 0;
}