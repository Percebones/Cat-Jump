/*
    Cat Jump - Índice do Código

    1. Inclusão de bibliotecas e macros
    2. Definição das estruturas (OBSTACULO, POMBO)
    3. Declaração das funções auxiliares
    4. Função calcularVelocidadePulo(): ajusta força do pulo conforme a tela
    5. Função reiniciarJogo(): reseta variáveis, música e objetos
    6. Função main():
       - Inicialização da janela, imagens, sons
       - Definição de variáveis, ponteiros e vetores
       - Loop principal do jogo:
         a. Atualização da música e dimensões da tela
         b. Controle de telas (menu, jogo, fim de jogo)
         c. Entrada do jogador (teclado: mover, pular, pausar, voltar, reiniciar)
         d. Lógica do jogo (movimento, pulo, colisões, pontuação, vida)
         e. Desenho (HUD, fundo, gato, obstáculos, pombos, menus)
    7. Finalização: descarrega recursos e fecha janela
*/

// Includes das bibliotecas
#include <iostream>
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>

// Deficnicoes do ambiente
#define NO_EXPAND(a) #a
#define STR(a) NO_EXPAND(a)
#define FIND_ASSET(AssetPath) STR(CONTENT_PATH) AssetPath
#define FIND_CONFIG(ConfigPath) STR(CONFIG_PATH) ConfigPath
#define MAX_OBSTACULOS 10
#define MAX_POMBOS 5

// Estrutura para representar obstáculos
typedef struct
{
    Vector2 pos;
    float velocidade;
    bool ativo;
} OBSTACULO;

// Estrutura para representar pombos
typedef struct
{
    Vector2 pos;
    bool ativo;
    float velocidade = 1.5;
} POMBO;

void salvarPontuacao(int pontos)
{
    std::ofstream arquivo("melhorPontuacao.txt");
    if (arquivo.is_open())
    {
        arquivo << pontos << "\n";
        arquivo.close();
    }
}

int carregarPontuacao()
{
    std::ifstream arquivo("melhorPontuacao.txt");
    int pontos = 0;
    if (arquivo.is_open())
    {
        arquivo >> pontos;
        arquivo.close();
    }
    return pontos;
}

// Funcao que reinicia o jogo
void reiniciarJogo(
    bool *reset,bool *novoMelhor ,int *pVida, int *pPontos, int *telaAtual,
    float *posicaoGatoX, float *posicaoGatoY, int alturaTela, int gatoAltura,
    Music levelMusica, Music menuMusica, double *inicioJogo,
    OBSTACULO obstaculos[], POMBO pombos[]);

// Funcao que calcula a velocidade do pula de acordo com a altura da tela
float calcularVelocidadePulo(int alturaTela)
{
    return (alturaTela >= 1080) ? -40.0f : -33.0f;
}

int main(void)
{
    // Inicia o janela e configura o seu tamanho de acordo com a resolucao do monitor e deixa em full screen
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
    Sound somPausa = LoadSound(FIND_ASSET("./musicas/pausa.mp3"));

    // Toca a musica do menu
    PlayMusicStream(menuMusica);

    // Carrega a logo
    Texture2D logo = LoadTexture(FIND_ASSET("./imagens/Logo.png"));

    // Carrega o plano de fundo do jogo e redimensiona para o monitor
    Image bgImage = LoadImage(FIND_ASSET("./imagens/bg.png"));
    ImageResize(&bgImage, larguraTela, alturaTela);
    Texture2D bg = LoadTextureFromImage(bgImage);

    // Carrega imagem do gato e redimensiona de acordo com a resolucao
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
    // posicaoGatoX e posicaoGatoY sao as posicoes do gato na tela e velocidade e a velocidade com que o gato anda
    // velocidadePulo seria a velocidade com que o gato sobe
    // gravidade e o valor utilizado na matematica para simular o gato caindo apos o pulo
    // inicioJogo var utilizada para receber tempoo do inicio do jogo e marcar o tempo decorrente da tentativa
    float posicaoGatoX = 0, posicaoGatoY = alturaTela - gato.height, velocidadePulo = 0, gravidade = 1, velocidade = 8.0f;
    int pulando = 0, telaAtual = 0, sair = false, pontos, vida, *pPontos = &pontos, *pVida = &vida;
    *pVida = 100, *pPontos = 0;
    char tempo[64];
    bool pausa = false, reset = false, novoMelhor = false;
    double inicioJogo;
    float menuBoxX, menuBoxY;
    OBSTACULO obstaculos[MAX_OBSTACULOS] = {0};
    POMBO pombos[MAX_POMBOS] = {0};

    // Inicio do Loop
    while (!WindowShouldClose() && !sair)
    {

        larguraTela = GetScreenWidth();
        alturaTela = GetScreenHeight();

        // Funcoes do Raylib que prepara as musicas pra usadas
        UpdateMusicStream(menuMusica);
        UpdateMusicStream(levelMusica);

        // Configurando a vida pra poder ser desenhada na tela
        char textoVida[64], textoPontos[3];
        sprintf(textoVida, "Vida: %d", *pVida);
        sprintf(textoPontos, "Pontos: %d", *pPontos);

        if (!pausa)
            ;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (pausa)
        {
            PauseMusicStream(levelMusica);
        }
        else
        {
            ResumeMusicStream(levelMusica);
        }

        switch (telaAtual)
        {
        case 0:
        {

            if (reset == true)
            {
                // Chamando funcao que reinicia o jogo
                reiniciarJogo(
                    &reset,&novoMelhor, pVida, pPontos, &telaAtual,
                    &posicaoGatoX, &posicaoGatoY, alturaTela, gato.height,
                    levelMusica, menuMusica, &inicioJogo,
                    obstaculos, pombos);
            }
            else
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
                menuBoxX = (larguraTela - 310) / 2.0f; // centralizar menu horizontalmente (largura 310)
                menuBoxY = logoY + logo.height + 20;   // 20 px abaixo da logo

                // Desenhar a logo
                DrawTexture(logo, logoX, logoY, RAYWHITE);

                // Desenhar o menu usando menuBoxX, menuBoxY
                DrawRectangle(menuBoxX, menuBoxY, 350, 180, ColorAlpha(WHITE, 0.7f));
                DrawRectangleLines(menuBoxX, menuBoxY, 350, 180, BLACK);
                DrawText("Menu Principal", menuBoxX + 10, menuBoxY + 10, 40, BLACK);
                DrawText("1 - Jogar", menuBoxX + 10, menuBoxY + 60, 30, BLACK);
                DrawText("2 - Melhor Pontuação", menuBoxX + 10, menuBoxY + 100, 30, BLACK);
                DrawText("3 - Sair", menuBoxX + 10, menuBoxY + 140, 30, BLACK);

                if (IsKeyPressed(KEY_TWO))
                {
                    telaAtual = 4;
                }

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
                if (IsKeyPressed(KEY_THREE))
                    sair = true;
                break;
            }
        }
        case 1:
        {
            // Pausa o jogo e a musica
            if (IsKeyPressed(KEY_P))
            {
                PlaySound(somPausa);
                pausa = !pausa;
            }

            // Se o jogo nao estiver pausado roda a logica
            if (!pausa)
            {
                // Configuracao para exibir o timer na tela corretamente
                double tempoAtual = GetTime() - inicioJogo;
                sprintf(tempo, "Tempo: %.0fs", tempoAtual);

                // Condicoes que fazem o gato ir para a esquerda(Aumentando o ou diminuindo o posicaoGatoX do gato)
                if (IsKeyDown(KEY_D) && posicaoGatoX < larguraTela - gato.width)
                    posicaoGatoX += velocidade;
                if (IsKeyDown(KEY_A) && posicaoGatoX > 0)
                    posicaoGatoX -= velocidade;

                // Verifica se a tecla de pulo esta pressionada e inicia o pulo
                if (!pulando && IsKeyPressed(KEY_SPACE))
                {

                    velocidadePulo = calcularVelocidadePulo(alturaTela);
                    pulando = 1;
                }
                // Faz o gato voltar para o chao
                if (pulando)
                {
                    velocidadePulo += gravidade;
                    posicaoGatoY += velocidadePulo;
                    if (posicaoGatoY >= alturaTela - gato.height)
                    {
                        posicaoGatoY = alturaTela - gato.height;
                        velocidadePulo = 0;
                        pulando = 0;
                    }
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
            DrawTexture(gato, (int)posicaoGatoX, (int)posicaoGatoY, WHITE);

            // Desenha o pausado na tela e toca o som do pause
            if (pausa)
            {
                DrawText("PAUSADO", menuBoxX - 50, 0, 100, RED);
            }

            // Se pause for false ira desenhar os obstaculos e os pombos
            if (!pausa)
            {
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
                        pombos[i].pos.x -= pombos[i].velocidade; // Move o pombo para a esquerda

                        DrawTexture(pomboImg, pombos[i].pos.x, pombos[i].pos.y, WHITE);

                        Rectangle gatoRec = {posicaoGatoX, posicaoGatoY, (float)gato.width, (float)gato.height};
                        Rectangle pomboRec = {pombos[i].pos.x, pombos[i].pos.y, (float)pomboImg.width, (float)pomboImg.height};

                        // Checa a colisao do gato com os pombos
                        if (CheckCollisionRecs(gatoRec, pomboRec))
                        {
                            *pPontos += 1;
                            // Deixa o jogo mais dificil apo 1 minuto diminuindo a quantidade de vida que o jogador ganha quando come os pombos para 5 pontos de vida (Eram 10 pontos de vida)
                            if (GetTime() - inicioJogo <= 60.0)
                            {
                                *pVida += 10;
                            }
                            else
                            {
                                *pVida += 5;
                            }

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
                        Rectangle gatoRec = {posicaoGatoX, posicaoGatoY, (float)gato.width, (float)gato.height};
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
            }

            // Condicao para quando a vida do gato chegar a 0 ele morre(ira prar a musica do menu e startar a musica do jogo)
            if (*pVida < 1)
            {
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

            // Reinicia o jogo
            if (IsKeyPressed(KEY_R))
            {
                reset = true;
                pausa = false;
                StopMusicStream(levelMusica);
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
            Rectangle rec; // x=100, y=100, largura=200, altura=150
            rec.x = larguraTela / 2.5f + 90;
            rec.y = alturaTela / 2 - 190;
            rec.width = 450;
            rec.height = 150;

            DrawTexture(bg, 0, 0, RAYWHITE);
            DrawRectangleRec(rec, ColorAlpha(WHITE, 0.7f));
            DrawRectangleLinesEx(rec, 1, BLACK);
            DrawText("Pressione BACKSPACE para voltar", larguraTela / 1.9 - MeasureText("Pressione BACKSPACE para voltar", 30) / 2, 30, 30, BLACK);
            DrawText("Pressione R para reiniciar", larguraTela / 1.9 - MeasureText("Pressione R para reiniciar", 30) / 2, 80, 30, BLACK);
            // Textos dentro da caixa com espaçamentos relativos ao fimBox
            DrawText("Fim de Jogo", (int)(rec.x + rec.width / 2 - MeasureText("Fim de Jogo", 60) / 2), (int)(rec.y - 70), 60, BLACK);
            DrawText(tempo,
                     rec.x + (rec.width - MeasureText(tempo, 40)) / 2,
                     rec.y + rec.height / 2 - 60, // centralizado verticalmente, ajusta conforme precisar
                     40,
                     BLACK); // Textos dentro da caixa com espaçamentos relativos ao fimBox
            DrawText(textoPontos,
                     rec.x + (rec.width - MeasureText(textoPontos, 40)) / 2,
                     rec.y + rec.height / 2 - 10, // centralizado verticalmente, ajusta conforme precisar
                     40,
                     BLACK);

            if (*pPontos > carregarPontuacao())
            {
                salvarPontuacao(*pPontos);
                novoMelhor = true;
            }

            if (novoMelhor)
            {
                DrawText("Nova Melhor Pontuação",
                         rec.x + (rec.width - MeasureText("Nova Melhor Pontuação", 40)) / 2,
                         rec.y + rec.height / 2 + 100, // centralizado verticalmente, ajusta conforme precisar
                         40,
                         BLACK);
            }

            if (IsKeyPressed(KEY_BACKSPACE))
            {
                *pPontos = 0;
                *pVida = 100;
                telaAtual = 0;
                novoMelhor = false;
            }

            if (IsKeyPressed(KEY_R))
            {
                // Chamando funcao que reinicia o jogo
                reiniciarJogo(
                    &reset,&novoMelhor, pVida, pPontos, &telaAtual,
                    &posicaoGatoX, &posicaoGatoY, alturaTela, gato.height,
                    levelMusica, menuMusica, &inicioJogo,
                    obstaculos, pombos);
            }
            break;
        }

            // Condicao que vai para o menu de Melhor Pontuacao
        case 4:
        {
            Rectangle rec; // x=100, y=100, largura=200, altura=150
            rec.x = larguraTela / 2.5f + 90;
            rec.y = alturaTela / 2 - 190;
            rec.width = 450;
            rec.height = 150;
            DrawTexture(bg, 0, 0, RAYWHITE);
            DrawRectangleRec(rec, ColorAlpha(WHITE, 0.7f));
            DrawRectangleLinesEx(rec, 1, BLACK);
            std::string textoPontuacao = "Sua Melhor Pontuação: " + std::to_string(carregarPontuacao());
            DrawText(textoPontuacao.c_str(),
                     rec.x + (rec.width - MeasureText(textoPontuacao.c_str(), 30)) / 2,
                     rec.y + rec.height / 2 - 15, // centralizado verticalmente, ajusta conforme precisar
                     30,
                     BLACK); // Textos dentro da caixa com espaçamentos relativos ao fimBox
            DrawText("Pressione BACKSPACE para voltar", larguraTela / 2 - MeasureText("Pressione BACKSPACE para voltar", 30) / 2, 30, 30, BLACK);

            if (IsKeyPressed(KEY_BACKSPACE))
            {
                telaAtual = 0;
            }
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
void reiniciarJogo(
    bool *reset, bool *novoMelhor, int *pVida, int *pPontos, int *telaAtual,
    float *posicaoGatoX, float *posicaoGatoY, int alturaTela, int gatoAltura,
    Music levelMusica, Music menuMusica, double *inicioJogo,
    OBSTACULO obstaculos[], POMBO pombos[])
{
    *novoMelhor = false;
    *reset = false;
    *pVida = 100;
    *pPontos = 0;
    *telaAtual = 1;
    *posicaoGatoX = 0;
    *posicaoGatoY = alturaTela - gatoAltura;
    StopMusicStream(menuMusica);
    PlayMusicStream(levelMusica);
    *inicioJogo = GetTime();

    for (int i = 0; i < MAX_OBSTACULOS; i++)
        obstaculos[i].ativo = false;

    for (int i = 0; i < MAX_POMBOS; i++)
        pombos[i].ativo = false;
}
