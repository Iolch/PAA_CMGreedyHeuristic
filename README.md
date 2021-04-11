# PAA_ProjetoCliqueMaximo1Unidade
## RUN

```
g++ -Wall -std=c++14 main.cpp -o ./build/main
./build/main ./graphs/nome.mtx limite

```
 - Limite deve ser um inteiro. 
- A lógica por trás do limite é: Caso façam mais de x iterações em que o cliqueMáximo não é modificado, pare de procurar. Esse limite deve levar em consideração a densidade de um grafo. 
