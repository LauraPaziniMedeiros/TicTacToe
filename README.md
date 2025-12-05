# Tic Tac Toe Evolutivo

> 🇺🇸 English | [🇧🇷 Versão em Português](#tic-tac-toe-evolutivo-1)

## Overview

This project implements a **Tic Tac Toe** evolutive genetic algorithm.

It was designed to demonstrate how an agent can "learn" to play a perfect game starting from zero knowledge, purely through evolutionary processes (mutation, crossover, and natural selection), and allows users to challenge the trained AI.

### Features
- **Genetic Training**: Bots evolve over generations playing against each other or against a perfect Minimax player.
- **Canonical State Optimization**: Uses board symmetries (rotations and reflections) to drastically reduce the learning space.
- **Interactive Mode**: Allows a human player to challenge the evolved bot.
- **Persistence**: Save and load the bot's "brain" (genomes) to text files.

---

## How It Works

The project is built in C++ and consists of several modules that manage the board, the bots, and the evolutionary logic.

### The "Brain" (Genome)
Unlike the Minimax algorithm, which calculates the best move in real-time by looking ahead, the Genetic Bot relies on a memory map. It stores a score for every possible move in every possible board state.
- **Good outcome (Win/Draw):** The score for that move increases.
- **Bad outcome (Loss):** The score for that move decreases.

### Canonical Optimization
To make learning faster, the bot understands that a rotated board is effectively the same board.
Instead of learning thousands of states, it converts any board to its "Canonical" form (the lexicographically smallest version among all 8 symmetries). This reduces the state space significantly.

### Evolution
1. **Population**: A group of bots with random behaviors.
2. **Selection**: Bots play matches. Winners survive/reproduce; losers are discarded.
3. **Crossover & Mutation**: The best strategies are combined, and random mutations are introduced to discover new moves.

---

# Usage / Compilation

### Prerequisites
- A C++ Compiler (g++)
- Make (optional, but recommended)

### Compiling
Use the provided `makefile` to compile the project easily:

```bash
make
````

Or manually via g++:

```bash
g++ Board.cpp Bot.cpp Optimal_algorithm.cpp Play.cpp population.cpp main.cpp -o a -Wall
```

### Running

Run the generated executable:

```bash
./a
```

### Menu Options

1.  **Train Population**: Evolves the bots. You can choose:
      - *Bot vs Bot*: Evolution through self-play.
      - *Bot vs Minimax*: Evolution by playing against a perfect teacher.
2.  **Play vs Bot**: Load a saved genome (e.g., `X0.txt`) and try to beat the AI.

-----

## Authors:

This project was developed to explore Evolutionary Computing concepts applied to Game Theory.

-----

# Tic Tac Toe Evolutivo

> 🇧🇷 Português | [🇺🇸 English Version](https://www.google.com/search?q=%23tic-tac-toe-evolutivo)

## Visão Geral

Este projeto implementa um algoritimo evolutivo capaz de jogar o **Jogo da Velha**.

Para isso, seguindo um modelo semelhante à evolução real, o algoritmo treina contra um jogador ideal (algoritmo Minimax) e aprende a cada rodada quais as jogadas mais assertivas para aquela situação.

### Funcionalidades

  - **Treinamento Genético**: Bots evoluem ao longo de gerações jogando uns contra os outros ou contra um jogador Minimax perfeito.
  - **Otimização Canônica**: Utiliza simetrias do tabuleiro (rotações e reflexões) para reduzir drasticamente o espaço de aprendizado.
  - **Modo Interativo**: Permite que um jogador humano desafie o bot evoluído.
  - **Persistência**: Salva e carrega os genomas do bot em arquivos de texto.

-----

# COLOCAR O VÍDEO

## Como Funciona

O projeto é construído em C++ e consiste em vários módulos que gerenciam o tabuleiro, os bots e a lógica evolutiva.

### O genoma

Ao contrário do algoritmo Minimax, que calcula a melhor jogada em tempo real prevendo o futuro, o Bot Genético baseia-se em um mapa de memória. Ele armazena uma pontuação para cada jogada possível em cada estado do tabuleiro.

No treinamento

  - **Bom resultado (Vitória/Empate):** A pontuação para aquela jogada aumenta.
  - **Mau resultado (Derrota):** A pontuação para aquela jogada diminui.

### Otimização Canônica

Para tornar o aprendizado mais rápido, o bot entende que um tabuleiro rotacionado é efetivamente o mesmo tabuleiro. Então,
em vez de aprender milhares de estados, ele converte qualquer tabuleiro para sua forma "Canônica" (a versão lexicograficamente menor entre todas as 8 simetrias). Isso reduz significativamente a quantidade de dados necessários.

### Evolução

1.  **População**: Um grupo de bots com comportamentos inicialmente aleatórios.
2.  **Seleção**: Os bots jogam partidas. Vencedores se reproduzem, combinando seus genes com os de novos bots aleatórios.
3.  **Crossover e Mutação**: Após a reprodução (crossover) é adicionado um fator de mutação variável que altera alguns genes.

-----
# Instruções de Uso / Compilação

### Pré-requisitos

  - Um compilador C++ (g++)
  - Make (opcional, mas recomendado)

### Compilando

Use o `makefile` fornecido para compilar o projeto facilmente:

```bash
make
```

Ou manualmente via g++:

```bash
g++ Board.cpp Bot.cpp Optimal_algorithm.cpp Play.cpp population.cpp -o a -Wall -Werror
```

### Executando

Rode o executável gerado:

```bash
./a
```

Ou execute 
```bash
make run
```
que complia e já roda o código

### Opções do Menu

1.  **Treinar População**: Evolui os bots. Você pode escolher:
      - *Bot vs Bot*: Evolução através de partidas contra outros bots.
      - *Bot vs Minimax*: Usado para treinar um bot com um algoritmo ideal.
2.  **Jogar contra o Bot**: Tente vencer o bot.

-----

## Autores:

# Colocar o NUSP de todo mundo
Pedro Otavio Mantovani - N° USP: 16896987  
Bruna Izabel da Silva Pereira - N° USP: 15635441  
Laura Pazini Medeiros - N° USP: 15468452  
Gustavo Ramos Santos Pires - N° USP: 15458030