# Cat-Jump

Cat Jump é um jogo 2D feito em C++ usando a biblioteca [raylib](https://www.raylib.com/). Controle o gato para pular e se movimentar enquanto o tempo passa. 

## Como jogar

- Use as teclas **A** e **D** para mover o gato para esquerda e direita.
- Aperte **SPACE** para pular.
- Pressione **BACKSPACE** para voltar ao menu.
- No menu, escolha:
  - **1** para jogar,
  - **2** para sair do jogo.

## Requisitos

- w64devkit-1.20.0

## Como compilar

Compile usando MAKE: make run


```bash
gcc -o catjump main.c -lraylib -lm -lpthread -ldl -lrt -lX11
