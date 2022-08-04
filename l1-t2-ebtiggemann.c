//
// para compilar este programa manualmente em linux:
// gcc -Wall -o l1-t2-ebtiggemann l1-t2-ebtiggemann.c tela.c -lallegro_font -lallegro_color -lallegro_ttf -lallegro_primitives -lallegro
//


#include "tela.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define cinza tela_cria_cor(0.1,0.1,0.1)

typedef struct {
    int x1;
    int y1;
    int x2;
    int y2;
} botao;

int aleat(int min, int max){
    return rand()%(max-min)+min;
}

// o texto a seguir será impresso com o fundo na cor correspondente
void cor_fundo(int VERMELHO, int VERDE, int AZUL)
{
  printf("%c[48;2;%d;%d;%dm", 27, VERMELHO, VERDE, AZUL);
}
// o texto a seguir será impresso com os símbolos na cor correspondente
void cor_texto(int VERMELHO, int VERDE, int AZUL)
{
  printf("%c[38;2;%d;%d;%dm", 27, VERMELHO, VERDE, AZUL);
}
// o texto a seguir será impresso nas cores default
void cor_normal(void)
{
  printf("%c[0m", 27);
}

void cor_roxa_fundo_preto(){
    cor_fundo(0,0,0);
    cor_texto(180,0,255);
}

void desenha_botao(botao b, float grossura, int cor, int corint){
    tela_retangulo(b.x1,b.y1,b.x2,b.y2,grossura,cor,corint);
}

bool clicou_botao(botao b){
    int posy = tela_rato_y_clique(), posx = tela_rato_x_clique();
    if (b.x1<posx && posx<b.x2 && b.y1<posy && posy<b.y2){
        return true;
    } else return false;
}

void desenha_mouse(float x, float y){
    tela_linha(x+0.3,y+0.3,x+25,y+25,8,preto);
    tela_retangulo(x,y,x+20,y+5,3.5,preto,preto);
    tela_retangulo(x,y,x+5,y+20,3.5,preto,preto);
    tela_retangulo(x+3.5,y+3.5,x+17.5,y+5,1.5,branco,branco);
    tela_retangulo(x+3.5,y+3.5,x+5,y+17.5,1.5,branco,branco);
    tela_linha(x+3.5,y+3.5,x+25,y+25,3.5,branco);
}

void desenha_placar(int *placar){
    int contador = 0;
    for (int i = 1; i<4; i++){
        for (int j = 5; j<9; j++){
            tela_retangulo(j*100,i*100,j*100+100,i*100+100,3,branco,preto);
        }
    }
    tela_retangulo(800,200,900,400,3,branco,preto);
    tela_texto(550,150,20,branco,"PLACAR");
    tela_texto(650,150,10,verde,"Vitórias");
    tela_texto(750,150,10,vermelho,"Desistências");
    tela_texto(850,150,10,azul,"Empates");
    tela_texto(550,250,60,vermelho,"X");
    tela_texto(550,350,60,azul,"O");
    char s[10];
    for (int i = 2; i<4; i++){
        for (int j = 6; j<9; j++){
            if (contador>4) break;
            sprintf(s,"%d",placar[contador]);
            tela_texto(j*100+50,i*100+50,50,branco,s);
            contador++;
        }
    }
}

bool placar_existe(){
    FILE *placar;
    placar = fopen("placar", "r");
    if (placar == NULL){
        return false;
    } else {
        fclose(placar);
        return true;
    }
}

int cria_placar(){
    FILE *placar;
    placar = fopen("placar","w");
    if (placar==NULL){
        printf("Não foi possível abrir o arquivo 'placar' para escrita.\n");
        return 1;
    }
    fprintf(placar, "%d %d %d %d %d", 0, 0, 0, 0, 0);
    fclose(placar);
    return 0;
}

void le_placar(int placar[5]){
    FILE *arq = fopen("placar", "r");
    if (arq == NULL){
        printf("o arquivo não existe!\n");
    } else {
        fscanf(arq, "%d %d %d %d %d", &placar[0], &placar[1], &placar[2], &placar[3], &placar[4]);
    }
    fclose(arq);
}

void escreve_placar(int placar[5]){
    FILE *arq = fopen("placar", "w+");
    if (arq == NULL){
        printf("o arquivo não existe!\n");
    } else {
        fprintf(arq,"%d %d %d %d %d",placar[0], placar[1], placar[2], placar[3], placar[4]);
    }
    fclose(arq);
}

void confere (char jogador, char tabuleiro[3][3], bool *venceu, bool *empate, bool desistiu, bool *jogainici, int *placar){
    int contadorDiag = 0, contadorGeral = 0, contadorHori = 0, contadorVert = 0;
    for (int i = 0; i<3; i++){
        if (*venceu == true || *empate == true){
            break;
        }
        contadorHori = 0, contadorVert = 0;
        if (tabuleiro[i][i] == jogador) {
            contadorDiag++;
        } if (tabuleiro[0][2] == jogador && tabuleiro[1][1] == jogador && tabuleiro[2][0] == jogador){
            contadorDiag = 3;
        } if (contadorDiag == 3){
            *venceu = true;
        } for (int j = 0; j<3; j++){
            if (tabuleiro[i][j] != ' '){
                contadorGeral++;
            }
            if (tabuleiro[i][j] == jogador){
                contadorHori++;
                if (contadorHori == 3){
                    *venceu = true;
                }
            } if (tabuleiro[j][i] == jogador){
                contadorVert++;
                if (contadorVert == 3){
                    *venceu = true;
                }
            }
            if (*venceu == true || *empate == true){
                break;
            }
        }
        if (contadorGeral == 9){
            *empate = true;
        }
    }
        if (jogador == 'X'){
            if (desistiu == true){
                placar[1]++;
                *jogainici = true;
                return;
            } else if (*venceu == true){
                printf("\nXis VENCEU!\n");
                placar[0]++;
                *jogainici = false;
                return;
            } else if (*empate == true) {
                printf("\nEMPATE!\n");
                placar[2]++;
                *jogainici = *jogainici == false ? !*jogainici : *jogainici;
                return;
            }

        }
        else {
            if (desistiu == true){
                placar[4]++;
                *jogainici = false;
                return;
            } else if (*venceu == true){
                printf("\nBolinha VENCEU!\n");
                *jogainici = true;
                placar[3]++;
                return;
            } else if (*empate == true){
                placar[2]++;
                printf("\nEMPATE!\n");
                *jogainici = *jogainici == false ? !*jogainici : *jogainici;
                return;
            }
        }
    return;
}

void jogada (char jogador, char tabuleiro[3][3]){
    //jogador == 'X' ? printf("\nVez do Xis!\n") : printf("\nVez do Bolinha!\n");
    int posy = tela_rato_y_clique(), posx = tela_rato_x_clique();
    tabuleiro[posy/100-1][posx/100-1] = jogador;
}

void imprimetab (char m[3][3]){
    int j = 0, i = 0;
    for (i=1;i<4;i++){
        for (j=1;j<4;j++){
            int posx = tela_rato_x(), posy = tela_rato_y();
            if(m[i-1][j-1]==' '){
                tela_retangulo(j*100,i*100,j*100+100,i*100+100,10,branco,(posx>j*100 && posx<j*100+100 && posy>i*100 && posy<i*100+100) ? cinza : preto);
            } if(m[i-1][j-1]=='O'){
                tela_retangulo(j*100,i*100,j*100+100,i*100+100,10,branco,preto);
                tela_circulo(j*100+50,i*100+50,30,10,azul,transparente);
            } if(m[i-1][j-1]=='X'){
                tela_retangulo(j*100,i*100,j*100+100,i*100+100,10,branco,preto);
                tela_linha(j*100+20,i*100+20,j*100+80,i*100+80,10,vermelho);
                tela_linha(j*100+80,i*100+20,j*100+20,i*100+80,10,vermelho);
            } 
        }
    }
}

void partida(char jogador1, char jogador2, bool *venceu, bool *empate, bool *desistiu, bool *jogainici, botao fecha, bool *fechou, int *placar){
    botao desiste = {750,350,950,450};
    *venceu = false;
    *empate = false;
    *desistiu = false;
    bool proximo = jogador1 == 'X' ? false : true;
    char jogaagora = jogador1;
    char tabuleiro[3][3] = {{' ',' ',' '}, {' ',' ',' '}, {' ',' ',' '}};
    while (*venceu == false && *empate == false && *desistiu == false){
        imprimetab(tabuleiro);
        desenha_botao(fecha, 1, vermelho, vermelho);
        tela_linha(fecha.x1+7,fecha.y1+2,fecha.x2-7,fecha.y2-2,1,branco);
        tela_linha(fecha.x1+7,fecha.y2-2,fecha.x2-7,fecha.y1+2,1,branco);
        desenha_botao(desiste, 2, vermelho, amarelo);
        desenha_placar(placar);
        desenha_mouse(tela_rato_x(),tela_rato_y());
        if(tela_rato_clicado()){
            int posy = tela_rato_y_clique(), posx = tela_rato_x_clique();
            if (clicou_botao(fecha)){
                *fechou = true;
                return;
            }
            if (clicou_botao(desiste)){
                *desistiu = true;
                confere(jogaagora, tabuleiro, venceu, empate, desistiu, jogainici, placar);
                if (*desistiu == true || *venceu == true || *empate == true){
                    imprimetab(tabuleiro);
                    tela_atualiza();
                    break;
                }
            }
            if (100<posy && posy<400 && 100<posx && posx<400 && tabuleiro[posy/100-1][posx/100-1] == ' '){
                jogada (jogaagora, tabuleiro);
                confere(jogaagora, tabuleiro, venceu, empate, *desistiu, jogainici, placar);
                if (*desistiu == true || *venceu == true || *empate == true){
                    imprimetab(tabuleiro);
                    tela_atualiza();
                    break;
                }
                proximo = !proximo;
                jogaagora = proximo == false ? 'X' : 'O';
            }
        }
        tela_atualiza();
    }
}

int main (){
    srand(time(0));
    if (placar_existe() == false) cria_placar();
    tela_inicio(1000,500, "sus");
    int rodada = 1;
    botao fecha = {950,25,975,40};
    bool venceu = false, empate = false, desistiu = false, jogainici = false, fechou = false;
    jogainici = aleat(0,3) == 1 ? false : true;
    char jogadorinicial, segundojogador;
    printf("Bem vindos! Eis o placar:\n");
    int placar[5];
    le_placar(placar);
    while (fechou == false){
        jogadorinicial = jogainici == false ? 'X' : 'O';
        segundojogador = jogadorinicial == 'X' ? 'O' : 'X';
        partida(jogadorinicial, segundojogador, &venceu, &empate, &desistiu, &jogainici, fecha, &fechou, placar);
        rodada++;
    }
    escreve_placar(placar);
    tela_fim();
    printf("\nO jogo acabou! Eis o placar final:\n");
    return 0;

}