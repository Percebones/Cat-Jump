#include <iostream>
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>

#define NO_EXPAND(a) #a
#define STR(a) NO_EXPAND(a)
#define FIND_ASSET(AssetPath) STR(CONTENT_PATH) AssetPath
#define FIND_CONFIG(ConfigPath) STR(CONFIG_PATH) ConfigPath

// Estrutura para representar obstáculos
typedef struct
{
    Vector2 pos;
    float velocidade;
    bool ativo;
} Obstaculo;

// Estrutura para representar pombos
typedef struct
{
    Vector2 pos;
    bool ativo;
    float velocidade = 1.5;
} Pombo;

#define MAX_OBSTACULOS 10
#define MAX_POMBOS 5

int main(void)
{

    //Inicia o janela e configura o seu tamanho de acordo com a resolucao do monitor e deixa em full screen
    InitWindow(600, 600, "Cat Jump");
    int larguraTela = GetMonitorWidth(0);
    int alturaTela = GetMonitorHeight(0);
    SetWindowState(FLAG_BORDERLESS_WINDOWED_MODE);
    SetWindowSize(larguraTela, alturaTela);
    SetTargetFPS(165);

    // Seta o Icone da janela
    Image ico = LoadImage(FIND_ASSET("./imagens/Logo.png"));
    SetWindowIcon(ico);
    UnloadImage(ico);

    // Inicia as funcoes de audio
    InitAudioDevice();
    // Carrega as musicas do menu e do jogo
    Music menuMusica = LoadMusicStream(FIND_ASSET("./musicas/menu.mp3")), levelMusica = LoadMusicStream(FIND_ASSET("./musicas/level.mp3"));
    // Toca a musica do menu
    PlayMusicStream(menuMusica);
    // Carrega a logo
    Texture2D logo = LoadTexture(FIND_ASSET("./imagens/Logo.png"));

    // Carrega o plano de fundo do jogo e redimensiona para o monitor
    Image bgImage = LoadImage(FIND_ASSET("./imagens/bg.png"));
    ImageResize(&bgImage, larguraTela, alturaTela);
    Texture2D bg = LoadTextureFromImage(bgImage);

    Image player = LoadImage(FIND_ASSET("./imagens/gato.png"));
    int tamanhoGato = larguraTela / 6;
    ImageResize(&player, tamanhoGato, tamanhoGato);
    Texture2D gato = LoadTextureFromImage(player);

    // Carrega imagem da bola de neve e redimensiona de acordo com a resolucao
    Image obstaculo = LoadImage(FIND_ASSET("./imagens/obstaculo.png"));
    int tamanhoObstaculo = larguraTela / 26;
    ImageResize(&obstaculo, tamanhoObstaculo, tamanhoObstaculo);
    Texture2D obstaculoImg = LoadTextureFromImage(obstaculo);

    // Carrega imagem do pombo e redimensiona de acordo com a resolucao
    Image pombo = LoadImage(FIND_ASSET("./imagens/pombo.png"));
    int tamanhoPombo = larguraTela / 15;
    ImageResize(&pombo, tamanhoPombo, tamanhoPombo);
    Texture2D pomboImg = LoadTextureFromImage(pombo);

    // Variaveis e ponteiros usados pelo jogo e seus menus
    // posx e posy sao as posicoes do gato na tela e velocidade e a velocidade com que o gato anda
    // velY seria a velocidade com que o gato sobe e desce
    // pontos e seu ponteiro ainda nao tem usos
    float posx = 0, posy = alturaTela - gato.height, velY = 0, gravity = 1, velocidade = 8.0f;
    int pulando = 0, telaAtual = 0, sair = false, pontos, vida;
    int *pPontos = &pontos, *pVida = &vida;
    *pVida = 100;
    char tempo[64];

    double inicioJogo;

    Obstaculo obstaculos[MAX_OBSTACULOS] = {0};
    Pombo pombos[MAX_POMBOS] = {0};

    // Inicio do Loop
    while (!WindowShouldClose() && !sair)
    {
        larguraTela = GetScreenWidth();
        alturaTela = GetScreenHeight();
        // Funcoes do Raylib que prepara as musicas pra usadas
        UpdateMusicStream(menuMusica);
        UpdateMusicStream(levelMusica);

        // Configurando a vida pra poder ser desenhada na tela
        char textoVida[64];
        sprintf(textoVida, "Vida: %d", *pVida);
        char textoPontos[3];
        sprintf(textoPontos, "Pontos: %d", *pPontos);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (telaAtual)
        {
        case 0:
        {
            // Desenha o plano de fundo
            DrawTexture(bg, 0, 0, RAYWHITE);

            // Defina a largura e altura do menu
            float menuWidth = 310;
            float menuHeight = 150;

            // Defina a posição do menu (caixa do menu)
            Rectangle menuBox;
            menuBox.width = menuWidth;
            menuBox.height = menuHeight;

            // Centraliza o menu na tela (embaixo da logo)
            menuBox.x = larguraTela / 2.0f - menuBox.width / 2.0f;
            menuBox.y = alturaTela / 2.0f; // posição do menu na vertical

            // Posição da logo: centralizada horizontalmente com o menu, mas acima dele
            // Define a posição da logo
            float logoX = (larguraTela - logo.width) / 2.0f; // centralizada horizontalmente
            float logoY = 20;                                // um pequeno espaço do topo

            // Define a posição do menu, sempre abaixo da logo
            float menuBoxX = (larguraTela - 310) / 2.0f; // centralizar menu horizontalmente (largura 310)
            float menuBoxY = logoY + logo.height + 20;   // 20 px abaixo da logo

            // Desenhar a logo
            DrawTexture(logo, logoX, logoY, RAYWHITE);

            // Desenhar o menu usando menuBoxX, menuBoxY
            DrawRectangle(menuBoxX, menuBoxY, 310, 150, ColorAlpha(WHITE, 0.7f));
            DrawRectangleLines(menuBoxX, menuBoxY, 310, 150, BLACK);
            DrawText("Menu Principal", menuBoxX + 10, menuBoxY + 10, 40, BLACK);
            DrawText("1 - Jogar", menuBoxX + 10, menuBoxY + 60, 30, BLACK);
            DrawText("2 - Sair", menuBoxX + 10, menuBoxY + 100, 30, BLACK);

            // Condicao que entra no jogo(muda a tela para o jogo, para a musica do menu e inicia a do jogo)
            if (IsKeyPressed(KEY_ONE))
            {
                telaAtual = 1;
                StopMusicStream(menuMusica);
                PlayMusicStream(levelMusica);
                inicioJogo = GetTime();
                // Reset obstacles and pigeons when starting game
                for (int i = 0; i < MAX_OBSTACULOS; i++)
                    obstaculos[i].ativo = false;
                for (int i = 0; i < MAX_POMBOS; i++)
                    pombos[i].ativo = false;
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
            sprintf(tempo, "Tempo: %.0fs", tempoAtual);

            // Condicoes que fazem o gato ir para a esquerda(Aumentando o ou diminuindo o posx do gato)
            if (IsKeyDown(KEY_D) && posx < larguraTela - gato.width)
                posx += velocidade;
            if (IsKeyDown(KEY_A) && posx > 0)
                posx -= velocidade;

            // Verifica se a tecla de pulo esta pressionada e inicia o pulo
            if (!pulando && IsKeyPressed(KEY_SPACE))
            {

                velY = -40;
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
            // Define o retângulo para o bloco no canto superior esquerdo com margem
            Rectangle infoBox;
            infoBox.x = 10;
            infoBox.y = 10;
            DrawText("Use WASD para mover o gato", (int)infoBox.x + 10, (int)infoBox.y + 10, 40, BLACK);
            DrawText(textoVida, (int)infoBox.x + 10, (int)infoBox.y + 60, 40, BLACK);
            DrawText(tempo, (int)infoBox.x + 10, (int)infoBox.y + 110, 40, BLACK);
            DrawText(textoPontos, (int)infoBox.x + 10, (int)infoBox.y + 160, 40, BLACK);
            DrawText("Pressione BACKSPACE para voltar", (int)infoBox.x + 10, (int)infoBox.y + 210, 30, BLACK);

            // Desenha o gato
            DrawTexture(gato, (int)posx, (int)posy, WHITE);

            // Spawna pombos
            for (int i = 0; i < MAX_POMBOS; i++)
            {
                if (!pombos[i].ativo && GetRandomValue(0, 1000) < 5)
                {
                    pombos[i].ativo = true;
                    pombos[i].pos.x = larguraTela;                                     // Começa à direita da tela
                    pombos[i].pos.y = GetRandomValue(0, alturaTela - pomboImg.height); // Altura aleatória
                    pombos[i].velocidade = GetRandomValue(2, 6);                       // Velocidade para a esquerda
                }

                if (pombos[i].ativo)
                {
                    pombos[i].pos.x -= pombos[i].velocidade; // Move para a esquerda

                    DrawTexture(pomboImg, pombos[i].pos.x, pombos[i].pos.y, WHITE);

                    Rectangle gatoRec = {posx, posy, (float)gato.width, (float)gato.height};
                    Rectangle pomboRec = {pombos[i].pos.x, pombos[i].pos.y, (float)pomboImg.width, (float)pomboImg.height};

                    if (CheckCollisionRecs(gatoRec, pomboRec))
                    {
                        *pPontos += 1;
                        *pVida += 10;
                        if (*pVida > 100)
                            *pVida = 100;
                        pombos[i].ativo = false;
                    }

                    if (pombos[i].pos.x + pomboImg.width < 0)
                    {
                        pombos[i].ativo = false; // Saiu da tela pela esquerda
                    }
                }
            }

            // Spawna obstáculos aleatoriamente
            for (int i = 0; i < MAX_OBSTACULOS; i++)
            {
                if (!obstaculos[i].ativo && GetRandomValue(0, 1000) < 10)
                {
                    obstaculos[i].ativo = true;
                    obstaculos[i].pos.x = GetRandomValue(0, larguraTela - obstaculoImg.width);
                    obstaculos[i].pos.y = -obstaculoImg.height;
                    obstaculos[i].velocidade = GetRandomValue(4, 10);
                }

                if (obstaculos[i].ativo)
                {
                    obstaculos[i].pos.y += obstaculos[i].velocidade;

                    DrawTexture(obstaculoImg, obstaculos[i].pos.x, obstaculos[i].pos.y, WHITE);

                    // Colisão com o gato
                    Rectangle gatoRec = {posx, posy, (float)gato.width, (float)gato.height};
                    Rectangle obstRec = {obstaculos[i].pos.x, obstaculos[i].pos.y, (float)obstaculoImg.width, (float)obstaculoImg.height};

                    if (CheckCollisionRecs(gatoRec, obstRec))
                    {
                        *pVida -= 10;
                        obstaculos[i].ativo = false;
                    }

                    if (obstaculos[i].pos.y > alturaTela)
                    {
                        obstaculos[i].ativo = false;
                    }
                }
            }

            // Condicao para quando a vida do gato chegar a 0 ele morre(ira prar a musica do menu e startar a musica do jogo)
            if (*pVida < 1)
            {
                DrawText("Gato Morreu de fome", 100, 50, 40, BLACK);
                StopMusicStream(levelMusica);
                PlayMusicStream(menuMusica);
                telaAtual = 3;
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
            // Condicao que vai para o menu de Fim de Jogo
        case 3:
        {
            DrawTexture(bg, 0, 0, RAYWHITE);

            // Define um "caixa" para o conteúdo do menu
            Rectangle fimBox;
            fimBox.x = larguraTela / 2.5f + 90;
            fimBox.y = alturaTela / 2 - 190;
            fimBox.width = 310;
            fimBox.height = 150;

            // Texto título acima da caixa, centralizado em relação à caixa
            DrawText("Fim de Jogo", (int)(fimBox.x + fimBox.width / 2 - MeasureText("Fim de Jogo", 60) / 2), (int)(fimBox.y - 70), 60, BLACK);

            // Caixa de fundo semi-transparente
            DrawRectangleRec(fimBox, ColorAlpha(WHITE, 0.7f));
            DrawRectangleLinesEx(fimBox, 1, BLACK);

            // Textos dentro da caixa com espaçamentos relativos ao fimBox
            DrawText("Pressione BACKSPACE para voltar", 50, 100, 30, BLACK);
            DrawText(tempo, (int)(fimBox.x + 10), (int)(fimBox.y + 10), 40, BLACK);
            DrawText(textoPontos, (int)(fimBox.x + 10), (int)(fimBox.y + 70), 40, BLACK);

            if (IsKeyPressed(KEY_BACKSPACE))
            {
                *pPontos = 0;
                *pVida = 100;
                telaAtual = 0;
            }
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
    UnloadTexture(obstaculoImg);
    UnloadTexture(pomboImg);
    // Descarrega as musicas usadas no jogo
    UnloadMusicStream(menuMusica);
    UnloadMusicStream(levelMusica);

    // Desativa as duncoes de audio
    CloseAudioDevice();
    // Fecha a janela do jogo
    CloseWindow();

    return 0;
}