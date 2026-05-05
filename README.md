# TP3 - Ciência da Computação: Processos Assíncronos e Paralelos

Este projeto implementa estruturas de dados avançadas (Árvores AVL, Quadtrees, Tries) e algoritmos de ordenação externa paralelizados com OpenMP. Todo o ambiente é isolado via Docker para garantir a consistência dos compiladores e bibliotecas.

## Pré-requisitos
*   **Docker** instalado em sua máquina.

## Preparando os Dados (listagem.txt)
Para testar o K-Way Merge Paralelo (Questão 2) com dados reais, o algoritmo espera um arquivo de texto simulando a saída do TP1. Execute o comando abaixo na raiz do projeto para varrer o diretório raiz (`/`) da sua máquina e salvar os caminhos no arquivo `listagem.txt`:

```bash
find / -maxdepth 4 2>/dev/null > listagem.txt
```

## Passo a Passo para Rodar

### 1. Preparar o Ambiente (Build)
Antes de qualquer coisa, você precisa construir a imagem que contém o compilador `g++`, as bibliotecas de paralelismo e o editor `vi` .
```bash
chmod +x run.sh
./run.sh build
```

### 2. Desenvolvimento e Edição (VI)
Conforme exigido pelo projeto, os códigos devem ser manipulados dentro do container utilizando o editor **VI**. Para entrar no ambiente interativo:
```bash
./run.sh run
```
Dentro do container, você pode abrir qualquer arquivo:
*   `vi ex2.cpp`

### 3. Execução Automatizada
Para compilar os códigos C++ com as flags `-fopenmp` e rodar todos os exercícios de uma vez (incluindo os scripts Python), utilize o comando:
```bash
./run.sh all
```
Este comando acionará o `Makefile` que gerencia as dependências e garante que a Questão 2 e 3 utilizem o paralelismo correto .