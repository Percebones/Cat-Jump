#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    InitWindow(800, 600, "Cat Jump");
    InitAudioDevice();
    Music menuMusica = LoadMusicStream("menu.mp3"), levelMusica = LoadMusicStream("level.mp3");
    PlayMusicStream(menuMusica);

    int larguraTela = GetMonitorWidth(0);
    int alturaTela = GetMonitorHeight(0);
    SetWindowSize(larguraTela, alturaTela);
    SetWindowState(FLAG_BORDERLESS_WINDOWED_MODE);
    SetTargetFPS(165);

    Image bgImage = LoadImage("./bg.jpg");
    ImageResize(&bgImage, 2560, 1440);
    Texture2D bg = LoadTextureFromImage(bgImage);
    Texture2D gato = LoadTexture("./gato.png");

    float posx = 0, posy = 942, velY = 0, gravity = 1, velocidade = 8.0f;
    int pulando = 0, telaAtual = 0, sair = false, pontos, vida;
    int *pPontos = &pontos, *pVida = &vida;
    *pVida = 100;
    double inicioJogo;

    while (!WindowShouldClose() && !sair)
    {
        UpdateMusicStream(menuMusica);
        UpdateMusicStream(levelMusica);

        char textoVida[64];
        sprintf(textoVida, "Vida: %d", *pVida);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        switch (telaAtual)
        {
        case 0:
        {
            DrawTexture(bg, 0, 0, RAYWHITE);
            DrawText("Menu Principal", 50, 50, 40, BLACK);
            DrawText("1 - Jogar", 50, 120, 30, DARKGRAY);
            DrawText("2 - Configuracoes", 50, 170, 30, DARKGRAY);
            DrawText("3 - Sair", 50, 220, 30, DARKGRAY);

            if (IsKeyPressed(KEY_ONE))
            {
                telaAtual = 1;
                StopMusicStream(menuMusica);
                PlayMusicStream(levelMusica);
                inicioJogo = GetTime();
            }

            if (IsKeyPressed(KEY_TWO))
                telaAtual = 2;
            if (IsKeyPressed(KEY_THREE))
                sair = true;
            break;
        }
        case 1:
        {

            double tempoAtual = GetTime() - inicioJogo;
            char tempo[64];
            sprintf(tempo, "Tempo: %.0f segundos", tempoAtual);
            if (IsKeyDown(KEY_D) && posx < larguraTela - gato.width)
                posx += velocidade;
            if (IsKeyDown(KEY_A) && posx > 0)
                posx -= velocidade;
            if (!pulando && IsKeyPressed(KEY_SPACE))
            {
                velY = -30;
                pulando = 1;
            }
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
            DrawTexture(bg, 0, 0, RAYWHITE);
            DrawText("Use WASD para mover o gato", 50, 50, 40, BLACK);
            DrawText(textoVida, 50, 100, 40, BLACK);
            DrawText(tempo, 50, 150, 40, BLACK);
            DrawTexture(gato, (int)posx, (int)posy, WHITE);
            DrawText("Pressione BACKSPACE para voltar", 50, 200, 30, DARKGRAY);

            if (*pVida < 1)
            {
                DrawText("Gato Morreu de fome", 100, 50, 40, BLACK);
                StopMusicStream(levelMusica);
                PlayMusicStream(menuMusica);
                telaAtual = 0;
                *pVida = 100;
                break;
            }
            if (IsKeyPressed(KEY_BACKSPACE))
            {
                StopMusicStream(levelMusica);
                PlayMusicStream(menuMusica);
                telaAtual = 0;
                break;
            }
            break;
        }

        case 2:
        {
            DrawText("Configuracoes (placeholder)", 50, 50, 30, BLACK);
            DrawText("Pressione BACKSPACE para voltar", 50, 100, 30, DARKGRAY);
            if (IsKeyPressed(KEY_BACKSPACE))
                telaAtual = 0;
            break;
        }
        }

        EndDrawing();
    }

    UnloadTexture(gato);
    UnloadTexture(bg);
    UnloadMusicStream(menuMusica);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
