https://www.raylib.com/
https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux
Executavel em releases

# 🐍 Snake Game - Raylib C++

Um clone clássico do jogo da Cobrinha (Snake) desenvolvido em C++ utilizando a biblioteca gráfica [Raylib](https://www.raylib.com/). 

Este projeto foi criado com foco em mecânicas clássicas e código limpo, incluindo correções para problemas comuns do gênero (como o "suicídio por duplo clique") e um sistema de spawn inteligente para a comida.

## ✨ Funcionalidades

- **Jogabilidade Clássica:** Cresça comendo as maçãs vermelhas e não bata nas paredes ou no próprio corpo.
- **Dificuldade Progressiva:** O jogo fica sutilmente mais rápido a cada maçã consumida.
- **Spawn Inteligente (Safe Spawn):** As maçãs nunca nascem coladas nas bordas da tela, garantindo que o jogador sempre tenha espaço para manobrar.
- **Proteção de Input:** O sistema impede que o jogador inverta a direção acidentalmente em um único frame, corrigindo o famoso bug de "comer o próprio pescoço".
- **Sistema de Pontuação:** Acompanhe seus pontos no canto superior esquerdo da tela.

## 🎮 Controles

- **Setas Direcionais (Cima, Baixo, Esquerda, Direita):** Controlam a direção da cobra.
- **Espaço ou Enter:** Reinicia o jogo após um "Game Over".

## 🛠️ Pré-requisitos

Para compilar e rodar este jogo, você precisará de:
1. Um compilador C++ (como `g++` ou `clang`).
2. A biblioteca **Raylib** instalada no seu sistema.
3. CMakeLists.txt configurado

## 🚀 Como Compilar e Executar

Abra o terminal na pasta onde o seu arquivo `main.cpp` está localizado e utilize o comando correspondente ao seu sistema operacional:

### Linux
```bash
g++ main.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o snake_game
