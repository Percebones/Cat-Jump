#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    // Inicia o janela e configura o seu tamanho de acordo com a resolucao do monitor e deixa em full screen
    InitWindow(800, 600, "Cat Jump");

    int larguraTela = GetMonitorWidth(0);
    int alturaTela = GetMonitorHeight(0);
    SetWindowSize(larguraTela, alturaTela);
    SetWindowState(FLAG_BORDERLESS_WINDOWED_MODE);
    SetTargetFPS(165);

    // Inicia as funcoes de audio
    InitAudioDevice();
    // Carrega as musicas do menu e do jogo
    Music menuMusica = LoadMusicStream("menu.mp3"), levelMusica = LoadMusicStream("level.mp3");
    // Toca a musica do menu
    PlayMusicStream(menuMusica);
    Texture2D logo = LoadTexture("./Logo.png");

    Image bgImage = LoadImage("./bg.jpg");
    ImageResize(&bgImage, larguraTela, alturaTela);
    Texture2D bg = LoadTextureFromImage(bgImage);
    Texture2D gato = LoadTexture("./gato.png");

    // Variaveis e ponteiros usados pelo jogo e seus menus
    // posx e posy sao as posicoes do gato na tela e velocidade e a velocidade com que o gato anda
    // velY seria a velocidade com que o gato sobe e desce
    // pontos e seu ponteiro ainda nao tem usos
    float posx = 0, posy = alturaTela - gato.height, velY = 0, gravity = 1, velocidade = 8.0f;
    int pulando = 0, telaAtual = 0, sair = false, pontos, vida;
    int *pPontos = &pontos, *pVida = &vida;
    *pVida = 100;
    double inicioJogo;

    while (!WindowShouldClose() && !sair)
    {
        // Funcoes do Raylib que prepara as musicas pra usadas
        UpdateMusicStream(menuMusica);
        UpdateMusicStream(levelMusica);

        // Configurando a vida pra poder ser desenhada na tela
        char textoVida[64];
        sprintf(textoVida, "Vida: %d", *pVida);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (telaAtual)
        {
        case 0:
        {
            // Desenha o plano de fundo
            DrawTexture(bg, 0, 0, RAYWHITE);

            // Desenha a logo do jogo na tela
            DrawTexture(logo, larguraTela / 2.5, alturaTela / 3 - logo.height, RAYWHITE);

            // Tudo isso e o menu sendo desenhado na tela
            DrawRectangle(larguraTela / 2.5 + 90, alturaTela / 2 - 190, 310, 150, ColorAlpha(WHITE, 0.7f));
            DrawRectangleLines(larguraTela / 2.5 + 90, alturaTela / 2 - 190, 310, 150, BLACK);
            DrawText("Menu Principal", larguraTela / 2.5 + 100, alturaTela / 2 - 180, 40, BLACK);
            DrawText("1 - Jogar", larguraTela / 2.5 + 100, alturaTela / 2 - 130, 30, BLACK);
            DrawText("2 - Sair", larguraTela / 2.5 + 100, alturaTela / 2 - 80, 30, BLACK);

            // Condicao que entra no jogo(muda a tela para o jogo, para a musica do menu e inicia a do jogo)
            if (IsKeyPressed(KEY_ONE))
            {
                telaAtual = 1;
                StopMusicStream(menuMusica);
                PlayMusicStream(levelMusica);
                inicioJogo = GetTime();
            }
            // Condicao que Sai do jogo
            if (IsKeyPressed(KEY_TWO))
                sair = true;
            break;
        }
        case 1:
        {
            // Configuracao para exibir o timer na tela corretamente
            double tempoAtual = GetTime() - inicioJogo;
            char tempo[64];
            sprintf(tempo, "Tempo: %.0f segundos", tempoAtual);

            // Condicoes que fazem o gator ir para a esquerda(Aumentando o ou diminuindo o posx do gato)
            if (IsKeyDown(KEY_D) && posx < larguraTela - gato.width)
                posx += velocidade;
            if (IsKeyDown(KEY_A) && posx > 0)
                posx -= velocidade;

            // Verifica se a tecla de pulo esta pressionada e inicia o pulo
            if (!pulando && IsKeyPressed(KEY_SPACE))
            {
                velY = -30;
                pulando = 1;
            }
            // Faz o gato voltar para o chao
            if (pulando)
            {
                velY += gravity;
                posy += velY;
                if (posy >= alturaTela - gato.height)
                {
                    posy = alturaTela - gato.height;
                    velY = 0;
                    pulando = 0;
                }
            }
            // Desenha a imagem de fundo do jogo
            DrawTexture(bg, 0, 0, RAYWHITE);
            // Desenha as informcoes que aparecem na tela
            DrawText("Use WASD para mover o gato", 50, 50, 40, BLACK);
            DrawText(textoVida, 50, 100, 40, BLACK);
            DrawText(tempo, 50, 150, 40, BLACK);
            DrawText("Pressione BACKSPACE para voltar", 50, 200, 30, BLACK);
            // Desenha o gato
            DrawTexture(gato, (int)posx, (int)posy, WHITE);

            // Condicao para quando a vida do gato chegar a 0 ele morre(ira prar a musica do menu e startar a musica do jogo)
            if (*pVida < 1)
            {
                DrawText("Gato Morreu de fome", 100, 50, 40, BLACK);
                StopMusicStream(levelMusica);
                PlayMusicStream(menuMusica);
                telaAtual = 0;
                *pVida = 100;
                break;
            }

            // Condicao para voltar ao menu(ira prar a musica do jogo e startar a musica do menu)
            if (IsKeyPressed(KEY_BACKSPACE))
            {
                StopMusicStream(levelMusica);
                PlayMusicStream(menuMusica);
                telaAtual = 0;
                break;
            }
            break;
        }
        // Condicao que volta para o menu
        case 2:
        {
            DrawText("Pressione BACKSPACE para voltar", 50, 100, 30, DARKGRAY);
            if (IsKeyPressed(KEY_BACKSPACE))
                telaAtual = 0;
            break;
        }
        }
        // Apaga tudo que foi desenhado na tela
        EndDrawing();
    }

    // Descarrega as imagems usadas no jogo
    UnloadTexture(gato);
    UnloadTexture(bg);
    UnloadTexture(logo);
    // Descarrega as musicas usadas no jogo
    UnloadMusicStream(menuMusica);
    UnloadMusicStream(levelMusica);

    // Desativa as duncoes de audio
    CloseAudioDevice();
    // Fecha a janela do jogo
    CloseWindow();

    return 0;
}
