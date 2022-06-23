#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

int aleat(int min, int max){
    return rand()%(max-min)+min;
}

// o texto a seguir será impresso com o fundo na cor correspondente
void cor_fundo(int vermelho, int verde, int azul)
{
  printf("%c[48;2;%d;%d;%dm", 27, vermelho, verde, azul);
}
// o texto a seguir será impresso com os símbolos na cor correspondente
void cor_texto(int vermelho, int verde, int azul)
{
  printf("%c[38;2;%d;%d;%dm", 27, vermelho, verde, azul);
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

void confere (char jogador, char tabuleiro[3][3], bool *venceu, bool *empate, bool desistiu, char placar[3][4], bool *jogainici){
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
                placar[1][3]++;
                placar[2][1]++;
                *jogainici = true;
                return;
            } else if (*venceu == true){
                printf("\nXis VENCEU!\n");
                placar[1][1]++;
                *jogainici = false;
                return;
            } else if (*empate == true) {
                printf("\nEMPATE!\n");
                placar[1][2]++;
                placar[2][2]++;
                *jogainici = *jogainici == false ? !*jogainici : *jogainici;
                return;
            }

        }
        else {
            if (desistiu == true){
                placar[2][3]++;
                placar[1][1]++;
                *jogainici = false;
                return;
            } else if (*venceu == true){
                printf("\nBolinha VENCEU!\n");
                *jogainici = true;
                placar[2][1]++;
                return;
            } else if (*empate == true){
                printf("\nEMPATE!\n");
                placar[2][2]++;
                placar[1][2]++;
                *jogainici = *jogainici == false ? !*jogainici : *jogainici;
                return;
            }
        }
    return;
}

void jogada (char jogador, bool *desistiu, char tabuleiro[3][3]){
    int lin, col;
    bool valido = false;
    jogador == 'X' ? printf("\nVez do Xis!\n") : printf("\nVez do Bolinha!\n");
    do{
        printf("Digite a linha e coluna onde deseja inserir sua jogada. Digite a linha 69 para desistir\n");
        scanf("%d", &lin);
        if(lin==69 || col==69){
            printf("Você desistiu!");
            *desistiu = true;
            break;
        } scanf("%d", &col);
        if ((tabuleiro[lin-1][col-1] != ' ') || ((lin<1 || lin>3 || col<1 || col>3) && (lin != 69 || col != 69))){
            printf("Jogada inválida");
        } else {
            tabuleiro[lin-1][col-1] = jogador;
            valido = true;
        }
    } while (valido == false);
}

void imprimeplac (char m[3][4]){
    for (int i = 0; i<3; i++){
        for (int j = 0; j<4; j++){
            if (m[i][j]=='V')
            {
                cor_texto(0,255,0);
            }
            if (m[i][j]=='E')
            {
                cor_texto(0,0,255);
            }
            if (m[i][j]=='D')
            {
                cor_texto(255,0,0);
            }
            
            printf(" %c",m[i][j]);
            cor_normal();
        }
        printf("\n");
    }
}

void imprimetab (char m[3][3]){
    int j = 0, i = 0;
    printf("\n");
    cor_roxa_fundo_preto();
    printf("┏━━━┳━━━┳━━━┓");
    cor_normal();
    printf("\n");
    for (i = 0; i<3; i++){
        cor_roxa_fundo_preto();
        printf("┃   ┃   ┃   ┃");
        cor_normal();
        printf("\n");
        for (j = 0; j<3; j++){
            cor_roxa_fundo_preto();
            printf("┃ %c ",m[i][j]);
            cor_normal();
        }
        cor_roxa_fundo_preto();
        printf("┃");
        cor_normal();
        printf("\n");
        cor_roxa_fundo_preto();
        printf("┃   ┃   ┃   ┃");
        cor_normal();
        printf("\n");
        if (i<2){
            cor_roxa_fundo_preto();
            printf("┣━━━╋━━━╋━━━┫");
            cor_normal();
            printf("\n");
        }
    }
    cor_roxa_fundo_preto();
    printf("┗━━━┻━━━┻━━━┛");
    cor_normal();
    printf("\n");
}

void partida(char jogador1, char jogador2, bool *venceu, bool *empate, bool *desistiu, char placar[3][4], bool *jogainici){
    *venceu = false;
    *empate = false;
    *desistiu = false;
    char tabuleiro[3][3] = {{' ',' ',' '}, {' ',' ',' '}, {' ',' ',' '}};
    while (*venceu == false && *empate == false && *desistiu == false){
        imprimetab(tabuleiro);
        jogada (jogador1, &*desistiu, tabuleiro);
        confere(jogador1, tabuleiro, &*venceu, &*empate, *desistiu, placar, &*jogainici);
        if (*desistiu == true || *venceu == true || *empate == true){
            break;
        }
        imprimetab(tabuleiro);
        jogada (jogador2, &*desistiu, tabuleiro);
        confere(jogador2, tabuleiro, &*venceu, &*empate, *desistiu, placar, &*jogainici);
        if (*desistiu == true || *venceu == true || *empate == true){
            break;
        }
    }
    imprimetab(tabuleiro);
}

int main (){
    srand(time(0));
    int rodada = 1;
    bool venceu = false, empate = false, desistiu = false, jogainici = false;
    jogainici = aleat(0,3) == 1 ? false : true;
    char jogadorinicial, segundojogador;
    char X = 'X', O = 'O';
    printf("Bem vindos! Eis o placar:\n");
    char placar [3][4] = {{' ','V','E','D'}, {'X', '0', '0', '0'}, {'O', '0', '0', '0'}};
    imprimeplac(placar);
    for (int i = 1; i != 0; rodada++){
        jogadorinicial = jogainici == false ? 'X' : 'O';
        segundojogador = jogadorinicial == 'X' ? 'O' : 'X';
        partida(jogadorinicial, segundojogador, &venceu, &empate, &desistiu, placar, &jogainici);
        imprimeplac(placar);
        printf("\nDigite 0 para parar de jogar, digite 1 para continuar:\n");
        scanf("%d", &i);
    }
    printf("\nO jogo acabou! Eis o placar final:\n");
    imprimeplac(placar);
    return 0;
}