# Cat Jump

Cat Jump é um jogo 2D feito em C usando a biblioteca [raylib](https://www.raylib.com/). Controle o gato para pular e se movimentar enquanto o tempo passa. 

## Como jogar

- Use as teclas **A** e **D** para mover o gato para esquerda e direita.
- Aperte **SPACE** para pular.
- Pressione **BACKSPACE** para voltar ao menu.
- No menu, escolha:
  - **1** para jogar,
  - **2** para configurações (ainda em desenvolvimento),
  - **3** para sair do jogo.

## Requisitos

- Biblioteca raylib instalada.
- Arquivos de música: `menu.mp3` e `level.mp3` na pasta do jogo.
- Imagens: `bg.jpg` (background) e `gato.png` (personagem).

## Como compilar

Compile usando gcc (ou seu compilador preferido) com as flags necessárias para raylib:

```bash
gcc -o catjump main.c -lraylib -lm -lpthread -ldl -lrt -lX11
