// para compilar este programa manualmente em linux:
// gcc -Wall -o l1-t3-ebtiggemann l1-t3-ebtiggemann.c tela.c -lallegro_font -lallegro_color -lallegro_ttf -lallegro_primitives -lallegro

#include "tela.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

// inclusive eu inclui (redundante) as bibliotecas do allegro
// aqui tambem, nao entendi pq mas nao tava compilando sem os includes

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define TELA_ALTURA 1080
#define TELA_LARGURA 1920
#define TELA_MEIO_X (TELA_LARGURA/2)
#define TELA_MEIO_Y (TELA_ALTURA/2)
#define TAM_MAX_PAL 160
#define TIRO_DIREITA false
#define TIRO_ESQUERDA true

ALLEGRO_FONT **tamanhosfontes;


int aleat(int min, int max){
    return rand()%(max-min)+min;
}

typedef struct{
    float x;
    float y;
} ponto;

typedef struct{
    int x1;
    int y1;
    int x2;
    int y2;
} botao;

typedef struct{
    float controle_de_spawn;
    char palavra[24];
    float tamanho;
    float velocidade;
    float distancia;
    float distanciaSpawn;
    float angulo;
    ALLEGRO_COLOR cor;
    bool na_tela;
    bool selecionada;
    bool vai_acertar;
    bool destruida;
    ponto pos;
} palavraStruct;

typedef struct{
    float x;
    float y;
    float raio;
    float angulo;
    ALLEGRO_COLOR cor;
    float grossura;
} miraStruct;

typedef struct{

} interface;

typedef struct{
    bool gerada;
    float poligono[100];
    float poligono2[100];
    float angulo1;
    float angulo2;
    float velocidade;
    float raioinicial;
    float raiofinal;
    ponto pos;
} explosao;

float distancia_entre_pontos(float p1, float p2){
    return p1-p2;
}

float hipotenusa(ponto p1, ponto p2){
    return sqrt(pow(distancia_entre_pontos(p1.x,p2.x),2)+pow(distancia_entre_pontos(p1.y,p2.y),2));
}

bool clicou_botao(botao b){
    int posy = tela_rato_y_clique(), posx = tela_rato_x_clique();
    if (b.x1<posx && posx<b.x2 && b.y1<posy && posy<b.y2){
        return true;
    } else return false;
}

void desenha_botao(botao b, float grossura, int cor, int corint, char *texto, int cortexto, int tamanho){
    tela_retangulo(b.x1,b.y1,b.x2,b.y2,grossura,cor,corint);
    tela_texto(b.x1+(b.x2-b.x1)/2,b.y1+(b.y2-b.y1)/2,tamanho,cortexto,texto);
}

void desenha_palavra(palavraStruct word){
    if (word.na_tela){
        al_draw_textf(tamanhosfontes[(int)(20*(word.tamanho/TAM_MAX_PAL))+5],al_map_rgb((127*(cos(word.tamanho)+1))*(word.tamanho/TAM_MAX_PAL),255-255*(word.tamanho/TAM_MAX_PAL),255-255*(word.tamanho/TAM_MAX_PAL)),word.pos.x+al_get_text_width(tamanhosfontes[(int)word.tamanho],word.palavra)/2,word.pos.y+5+al_get_font_line_height(tamanhosfontes[(int)word.tamanho])/2,ALLEGRO_ALIGN_RIGHT,"%.2fm",TAM_MAX_PAL-word.tamanho);
        if (word.selecionada){
            al_draw_text(tamanhosfontes[(int)word.tamanho], al_map_rgb(0,255,0), word.pos.x, word.pos.y-word.tamanho/2, ALLEGRO_ALIGN_CENTRE, word.palavra);
        } else {
            al_draw_text(tamanhosfontes[(int)word.tamanho], word.cor, word.pos.x, word.pos.y-word.tamanho/2, ALLEGRO_ALIGN_CENTRE, word.palavra);
        }
    }// tela_texto(word.pos.x,word.pos.y,word.tamanho,word.cor,word.palavra);
}

void desenha_mira(miraStruct mira){
    al_draw_arc(mira.x, mira.y, mira.raio, mira.angulo*(M_PI/180), 50*(M_PI/180), mira.cor, mira.grossura);
    al_draw_arc(mira.x, mira.y, mira.raio, (mira.angulo+180)*(M_PI/180), 50*(M_PI/180), mira.cor, mira.grossura);
}

void pega_da_matriz_char(char *pa, char **matriz, int nlinhasnivel){
    strcpy(pa,matriz[aleat(-1,nlinhasnivel)]);
}

palavraStruct constroi_palavra(char **matriz, int *i, int nlinhasnivel){
    *i += 1;
    palavraStruct p;
    pega_da_matriz_char(p.palavra,matriz,nlinhasnivel);
    p.controle_de_spawn = aleat(-5,0)-2**i;
    p.tamanho = 1;
    p.cor = al_map_rgba(0,0,0,0);
    p.na_tela = false;
    p.selecionada = false;
    p.vai_acertar = aleat(0,100) <= 85 ? true : false;
    p.velocidade = aleat(60,100)/6;
    p.destruida = false;
    p.angulo = aleat(-1,361);
    p.distancia = p.vai_acertar ? aleat(TELA_MEIO_Y-200,TELA_MEIO_Y) : TELA_MEIO_Y+200;
    p.distanciaSpawn = p.distancia;
    p.pos.x = TELA_MEIO_X+cos((p.angulo)*M_PI/180)*p.distancia;
    p.pos.y = TELA_MEIO_Y+sin((p.angulo)*M_PI/180)*p.distancia;
    return p;
}

void altera_palavra(palavraStruct *p, int *vidas, bool *tem_selecao, float difftempo){
    p->na_tela = p->controle_de_spawn >= 0 && !p->destruida;
    if (p->na_tela){
        if (p->tamanho>=TAM_MAX_PAL) {
            if (p->vai_acertar){
                *vidas -=1;
                p->destruida = true;
                if (p->selecionada) {
                    *tem_selecao = false;
                }
                p->selecionada = false;
                return;
            } else {
                p->destruida = true;
                if (p->selecionada) {
                    *tem_selecao = false;
                }
                p->selecionada = false;
                return;
            }
        }
        p->tamanho += p->velocidade*difftempo;
        p->distancia = p->vai_acertar ? p->distanciaSpawn*((TAM_MAX_PAL-p->tamanho)/TAM_MAX_PAL) : p->distanciaSpawn*(p->tamanho)/TAM_MAX_PAL;
        p->pos.x = TELA_MEIO_X+cos((p->angulo)*M_PI/180)*p->distancia;
        p->pos.y = TELA_MEIO_Y+sin((p->angulo)*M_PI/180)*p->distancia;
        p->cor = p->vai_acertar ? al_map_rgba(255,255-255*(p->tamanho/TAM_MAX_PAL),255-255*(p->tamanho/TAM_MAX_PAL),255) : al_map_rgba(255*(p->tamanho/TAM_MAX_PAL),255*(p->tamanho/TAM_MAX_PAL),255*(p->tamanho/TAM_MAX_PAL),255);
    } else {
        p->controle_de_spawn += 1*difftempo;
        if (p->selecionada) {
            *tem_selecao = false;
        }
        p->selecionada = false;

    }
}

void desenha_mouse(){
    float x = tela_rato_x();
    float y = tela_rato_y();
    tela_circulo(x,y,10,1,branco,preto);
}

void desenha_vidas(){

}

void deleta_caractere(palavraStruct *words, bool *tem_selecao, int rodada, int *n, int nlinhasnivel, char **nivel){
    for (int i = 0; i<24; i++){
        words->palavra[i] = words->palavra[i+1];
    }
    if(words->palavra[1]=='\0'){
        words->destruida = true;
        *tem_selecao = false;
        words->selecionada = false;
        if (rodada==3){
            *words = constroi_palavra(nivel,n,nlinhasnivel);
        }
    }
}

void seleciona_palavra(palavraStruct *words, char c, bool *tem_selecao, int *selecao, int npalavras){
    for (int i = 0; i<npalavras+1; i++){
        if (c == words[i].palavra[0] && words[i].na_tela){
            *tem_selecao = true;
            *selecao = i;
            words[i].selecionada = true;
            return;
        } 
    }
}

typedef struct{
    float velocidade;
    ponto pontospawn;
    ponto fora1;
    ponto fora2;
    float angulo;
    float raiocentral;
    float raioexterno;
    bool tiro_atirado;
    bool lado_do_tiro;
    int palavra_selecionada_tiro;
    ponto alvo_tiro;
} linhaVel;

void criaTiros(linhaVel *tiros, palavraStruct *words, int selecao, bool lado){
    for (int i = 0; i<10; i ++){
        if (tiros[i].tiro_atirado == false){
            tiros[i].palavra_selecionada_tiro = selecao;
            tiros[i].tiro_atirado = true;
            tiros[i].lado_do_tiro = lado == TIRO_DIREITA ? false : true;
            tiros[i].velocidade = 1500;
            tiros[i].alvo_tiro.x = words[selecao].pos.x-al_get_text_width(tamanhosfontes[(int)words[selecao].tamanho],words[selecao].palavra)/2;
            tiros[i].alvo_tiro.y = words[selecao].pos.y;//+al_get_font_line_height(tamanhosfontes[(int)words[selecao].tamanho])/2;
            tiros[i].angulo = tiros[i].lado_do_tiro == TIRO_DIREITA ? atan2((TELA_ALTURA-words[selecao].pos.y),(TELA_LARGURA-tiros[i].alvo_tiro.x))*180/M_PI : atan2((TELA_ALTURA-words[selecao].pos.y),(0-words[selecao].pos.x))*180/M_PI; 
            tiros[i].raiocentral = tiros[i].lado_do_tiro == TIRO_DIREITA ? sqrt(pow(TELA_LARGURA-words[selecao].pos.x,2)+pow(TELA_ALTURA-words[selecao].pos.y,2)) : sqrt(pow(words[selecao].pos.x,2)+pow(TELA_ALTURA-words[selecao].pos.y,2));
            tiros[i].raioexterno = tiros[i].raiocentral;
            tiros[i].pontospawn.x = words[selecao].pos.x-al_get_text_width(tamanhosfontes[(int)words[selecao].tamanho],words[selecao].palavra)/2+cos(tiros[i].angulo*M_PI/180)*tiros[i].raiocentral;
            tiros[i].pontospawn.y = words[selecao].pos.y+sin(tiros[i].angulo*M_PI/180)*tiros[i].raiocentral;
            tiros[i].fora1.x = words[selecao].pos.x-al_get_text_width(tamanhosfontes[(int)words[selecao].tamanho],words[selecao].palavra)/2+cos((tiros[i].angulo-1)*M_PI/180)*tiros[i].raioexterno;
            tiros[i].fora1.y = words[selecao].pos.y+sin((tiros[i].angulo-1)*M_PI/180)*tiros[i].raioexterno;
            tiros[i].fora2.x = words[selecao].pos.x-al_get_text_width(tamanhosfontes[(int)words[selecao].tamanho],words[selecao].palavra)/2+cos((tiros[i].angulo+1)*M_PI/180)*tiros[i].raioexterno;
            tiros[i].fora2.y = words[selecao].pos.y+sin((tiros[i].angulo+1)*M_PI/180)*tiros[i].raioexterno;
            return;
        }
    }
}


void criaExplosao(explosao *explosoes, float x, float y, float raiomin, float raiomax){
    for (int i = 0; i<20 ; i++){
        if (explosoes[i].gerada == false){
            explosoes[i].gerada = true;
            explosoes[i].angulo1 = 0;
            explosoes[i].angulo2 = aleat(-1,361)*M_PI/180;
            explosoes[i].velocidade = aleat(1200,1500);
            explosoes[i].raioinicial = 0;
            explosoes[i].raiofinal = aleat(raiomin,raiomax);
            explosoes[i].pos.x = x;
            explosoes[i].pos.y = y+aleat(-20,20);
            return;
        }
    }
}

void alteraTiros(linhaVel *tiros, palavraStruct words, float difftempo, explosao *explosoes){
    for (int i = 0; i<10; i++){
        if (tiros[i].tiro_atirado){
            //tiros[i].angulo = tiros[i].lado_do_tiro == TIRO_DIREITA ? atan((TELA_LARGURA-words[tiros[i].palavra_selecionada_tiro].pos.x)/(TELA_ALTURA-words[tiros[i].palavra_selecionada_tiro].pos.y)) : atan((words[tiros[i].palavra_selecionada_tiro].pos.x)/(TELA_ALTURA-words[tiros[i].palavra_selecionada_tiro].pos.y));
            if (tiros[i].raiocentral>0){
                tiros[i].raiocentral -= tiros[i].raiocentral/2;
            } else{
                tiros[i].raiocentral = 0;
            } if (tiros[i].raiocentral<100 && tiros[i].raioexterno != tiros[i].raiocentral){
                tiros[i].raioexterno -= tiros[i].velocidade*difftempo*5;
            }
            tiros[i].pontospawn.x = tiros[i].alvo_tiro.x+cos(tiros[i].angulo*M_PI/180)*tiros[i].raiocentral;
            tiros[i].pontospawn.y = tiros[i].alvo_tiro.y+20+sin(tiros[i].angulo*M_PI/180)*tiros[i].raiocentral;
            tiros[i].fora1.x = tiros[i].alvo_tiro.x+cos((tiros[i].angulo-1)*M_PI/180)*tiros[i].raioexterno;
            tiros[i].fora1.y = tiros[i].alvo_tiro.y+20+sin((tiros[i].angulo-1)*M_PI/180)*tiros[i].raioexterno;
            tiros[i].fora2.x = tiros[i].alvo_tiro.x+cos((tiros[i].angulo+1)*M_PI/180)*tiros[i].raioexterno;
            tiros[i].fora2.y = tiros[i].alvo_tiro.y+20+sin((tiros[i].angulo+1)*M_PI/180)*tiros[i].raioexterno;
            if (tiros[i].raioexterno<=1){
                tiros[i].tiro_atirado = false;
                criaExplosao(explosoes,tiros[i].alvo_tiro.x,tiros[i].alvo_tiro.y,10+words.tamanho/5,20+words.tamanho/2);
            }
        }
    }
}

void alteraExplosao(explosao *explosoes, float difftempo){
    for (int i = 0; i<20; i++){
        if (explosoes[i].gerada == true){
            if (explosoes[i].raioinicial <= explosoes[i].raiofinal){
                explosoes[i].raioinicial += (explosoes[i].velocidade*difftempo)/5;
            } else if (explosoes[i].angulo1 <= 2*M_PI) {
                explosoes[i].angulo1 += (M_PI/180)*explosoes[i].velocidade*difftempo;
                al_calculate_arc(&explosoes[i].poligono[0], 2 * sizeof(float), explosoes[i].pos.x, explosoes[i].pos.y, explosoes[i].raiofinal , explosoes[i].raiofinal, M_PI-explosoes[i].angulo1/2, -(2*M_PI-explosoes[i].angulo1), 0, 25);
                al_calculate_arc(&explosoes[i].poligono[50], 2 * sizeof(float), explosoes[i].poligono[0], explosoes[i].pos.y, ((explosoes[i].poligono[24]-explosoes[i].poligono[0])*(explosoes[i].angulo1/(2*M_PI))) ,(explosoes[i].poligono[1]-explosoes[i].poligono[49])/2, 3*M_PI/2, M_PI, 0, 25);
                for (int j = 0; j<100 ; j+=2){
                    explosoes[i].poligono2[j] = explosoes[i].pos.x+(cos(explosoes[i].angulo2+atan2(explosoes[i].poligono[j+1]-explosoes[i].pos.y,explosoes[i].poligono[j]-explosoes[i].pos.x)))*sqrt(pow(distancia_entre_pontos(explosoes[i].poligono[j+1],explosoes[i].pos.y),2)+pow(distancia_entre_pontos(explosoes[i].poligono[j],explosoes[i].pos.x),2));
                    explosoes[i].poligono2[j+1] = explosoes[i].pos.y+(sin(explosoes[i].angulo2+atan2(explosoes[i].poligono[j+1]-explosoes[i].pos.y,explosoes[i].poligono[j]-explosoes[i].pos.x)))*sqrt(pow(distancia_entre_pontos(explosoes[i].poligono[j+1],explosoes[i].pos.y),2)+pow(distancia_entre_pontos(explosoes[i].poligono[j],explosoes[i].pos.x),2));
                }
            } else explosoes[i].gerada = false;
        }
    }
}

void desenhaExplosao(explosao *explosoes){
    for (int i = 0; i<20;i++){
        if (explosoes[i].gerada == true){
            if (explosoes[i].raioinicial <= explosoes[i].raiofinal){
                al_draw_filled_circle(explosoes[i].pos.x,explosoes[i].pos.y,explosoes[i].raioinicial,al_map_rgb(255,255,255));
            } else {
                al_draw_filled_polygon(explosoes[i].poligono2,50,al_map_rgb(255,255-140*(explosoes[i].angulo1/(2*M_PI)),255-255*(explosoes[i].angulo1/(2*M_PI))));
            }
        }
    }
}

void criaLinha(linhaVel *linhas){
    linhas->tiro_atirado = false;
    linhas->velocidade = aleat(100,200)*7;
    linhas->angulo = aleat(-1,361); 
    linhas->raiocentral = aleat(50,500);
    linhas->raioexterno = linhas->raiocentral;
    linhas->pontospawn.x = TELA_MEIO_X+cos((linhas->angulo-1)*M_PI/180)*linhas->raiocentral;
    linhas->pontospawn.y = TELA_MEIO_Y+sin((linhas->angulo-1)*M_PI/180)*linhas->raiocentral;
    linhas->fora1.x = TELA_MEIO_X+cos((linhas->angulo-1)*M_PI/180)*linhas->raioexterno;
    linhas->fora1.y = TELA_MEIO_Y+sin((linhas->angulo-1)*M_PI/180)*linhas->raioexterno;
    linhas->fora2.x = TELA_MEIO_X+cos((linhas->angulo+1)*M_PI/180)*linhas->raioexterno;
    linhas->fora2.y = TELA_MEIO_Y+sin((linhas->angulo+1)*M_PI/180)*linhas->raioexterno;
}

void alteraLinha(linhaVel *linhas, float difftempo, float wpm){
    for(int i = 0 ; i <50 ; i++){
        if (linhas[i].raiocentral>1110){
            criaLinha(&linhas[i]);
        } else {
            linhas[i].raioexterno += linhas[i].velocidade*wpm/2*difftempo;
            if (linhas[i].raioexterno>linhas[i].raiocentral+450){
                linhas[i].raiocentral += (linhas[i].velocidade*wpm/2+10)*difftempo;
            }
            linhas[i].pontospawn.x = TELA_MEIO_X+cos(linhas[i].angulo*M_PI/180)*linhas[i].raiocentral;
            linhas[i].pontospawn.y = TELA_MEIO_Y+sin(linhas[i].angulo*M_PI/180)*linhas[i].raiocentral;
            linhas[i].fora1.x = TELA_MEIO_X+cos((linhas[i].angulo-1)*M_PI/180)*linhas[i].raioexterno;
            linhas[i].fora1.y = TELA_MEIO_Y+sin((linhas[i].angulo-1)*M_PI/180)*linhas[i].raioexterno;
            linhas[i].fora2.x = TELA_MEIO_X+cos((linhas[i].angulo+1)*M_PI/180)*linhas[i].raioexterno;
            linhas[i].fora2.y = TELA_MEIO_Y+sin((linhas[i].angulo+1)*M_PI/180)*linhas[i].raioexterno;
        }
    }
}

void alteraMira (miraStruct *mira,bool tem_selecao, palavraStruct word,float difftempo){
    mira->grossura = 5;
    if (tem_selecao){
        mira->angulo += 250*difftempo;
        mira->cor = al_map_rgb(255,50,50);
        mira->raio = 15 + word.tamanho/5;
        mira->x = word.pos.x-al_get_text_width(tamanhosfontes[(int)word.tamanho],word.palavra)/2 - (distancia_entre_pontos(word.pos.x-al_get_text_width(tamanhosfontes[(int)word.tamanho],word.palavra)/2,mira->x)*8/10);
        mira->y = word.pos.y - (distancia_entre_pontos(word.pos.y,mira->y)*8/10);
    } else {
        mira->angulo += 100*difftempo;
        mira->cor = al_map_rgb(0,150,255);
        mira->raio = 35;
        mira->x = TELA_MEIO_X - (distancia_entre_pontos(TELA_MEIO_X,mira->x)*8/10);
        mira->y = TELA_MEIO_Y - (distancia_entre_pontos(TELA_MEIO_Y,mira->y)*8/10);
    }
}

void desenhaTiro (linhaVel *tiros){
    ALLEGRO_COLOR cor = al_map_rgb(255,0,0);
        for (int i = 0 ; i < 10 ; i++){
            if (tiros[i].tiro_atirado){
                al_draw_filled_triangle(tiros[i].fora1.x, tiros[i].fora1.y,tiros[i].fora2.x,tiros[i].fora2.y,tiros[i].pontospawn.x,tiros[i].pontospawn.y,cor);
            }
    }
}

void desenhaLinha (linhaVel *linhas, float wpm){
    ALLEGRO_COLOR cor = al_map_rgb_f((wpm/5),(wpm/5),(wpm/5));
        for (int i = 0 ; i < (int)wpm+5 ; i++){
            al_draw_filled_triangle(linhas[i].fora1.x, linhas[i].fora1.y,linhas[i].fora2.x,linhas[i].fora2.y,linhas[i].pontospawn.x,linhas[i].pontospawn.y,cor);
        // al_draw_circle(TELA_MEIO_X,TELA_MEIO_Y,linhas[i].raiocentral,al_map_rgb(255,0,255),1);
        // al_draw_circle(TELA_MEIO_X,TELA_MEIO_Y,linhas[i].raioexterno,al_map_rgb(255,0,255),1);
        // al_flip_display();
        // al_rest(0.3);
    }
}

void confere_rodada(palavraStruct *words, int npalavras, int *rodada, bool *rodada_acabou){
    for (int i = 0; i < npalavras+1; i++){
        if (!words[i].destruida) return;
    } *rodada += 1;
    *rodada_acabou = true;

}

void partida(char **nivel,int nlinhasnivel){
    tela_atualiza();
    double lasttempo;
    int rodada = 3;
    int vidas = 3;
    bool morreu = false;
    bool lado_tiro = false;
    while(!morreu){
        int teclas_certas = 0;
        int digitadas = 0;
        int selecao = 0;
        int n = 0;
        int npalavras = 20+rodada*5;
        float taxa_de_acerto = 0;
        double tempoinicio = relogio(), tempoagora;
        double wpm = 0;
        char c[1] = "\0";
        bool tem_selecao = false;
        bool rodada_acabou = false;
        palavraStruct words[npalavras+1];
        for (int i = 0; i < npalavras+1; i++){
            words[i]=constroi_palavra(nivel,&n,nlinhasnivel);
        }
        linhaVel linhas[50];
        for (int i = 0 ; i<50 ; i++){
            criaLinha(&linhas[i]);
        }
        linhaVel tiros[10];
        for (int i = 0 ; i<10 ; i++){
            tiros[i].tiro_atirado = false;
        }
        miraStruct mira;
        explosao explosoes[20];
        for (int i = 0 ; i < 20 ;  i++){
            explosoes[i].gerada = false;
        }
        while(!morreu && !rodada_acabou){
            lasttempo = tempoagora;
            tempoagora = relogio();
            c[0] = tela_tecla();
            if (c[0] != '\0' && c[0]!='\n'){
                digitadas++;
                if (!tem_selecao) {
                    seleciona_palavra(words, c[0], &tem_selecao, &selecao, npalavras);
                    n--;
                }
                if (tem_selecao && c[0] == words[selecao].palavra[0]){
                    criaTiros(tiros,words,selecao,lado_tiro);
                    deleta_caractere(&words[selecao],&tem_selecao,rodada,&n,nlinhasnivel,nivel);
                    teclas_certas++;
                    lado_tiro = !lado_tiro;
                }
            }
            desenhaLinha(linhas,wpm);
            desenhaTiro(tiros);
            alteraLinha(linhas,tempoagora-lasttempo,wpm);
            alteraTiros(tiros,words[selecao],tempoagora-lasttempo,explosoes);
            for (int i = npalavras; i >= 0; i--){
                desenha_palavra(words[i]);
                altera_palavra(&words[-i+npalavras], &vidas, &tem_selecao, tempoagora-lasttempo);
            }
            desenhaExplosao(explosoes);
            alteraExplosao(explosoes,tempoagora-lasttempo);
            wpm = teclas_certas/(tempoagora-tempoinicio);
            if(digitadas>0) taxa_de_acerto = ((float)teclas_certas/(float)digitadas);
            al_draw_textf(tamanhosfontes[20], al_map_rgb(128,255,0), 200, 800-20/2, ALLEGRO_ALIGN_CENTRE, "Teclas por minuto: %.2f",wpm*60.0);
            al_draw_textf(tamanhosfontes[20], al_map_rgb(128,255,0), 200, 600-20/2, ALLEGRO_ALIGN_CENTRE, "Taxa de acerto: %.2f",taxa_de_acerto*100.0);
            alteraMira(&mira,tem_selecao,words[selecao],tempoagora-lasttempo);
            desenha_mira(mira);
            if (vidas <= 0) {
                morreu = false;
            } if (c[0] == '\n') {
                al_draw_filled_rectangle(0,0,TELA_LARGURA,TELA_ALTURA, al_map_rgba(0,0,0,180));
                al_draw_text(tamanhosfontes[50], al_map_rgb(255,255,255),TELA_MEIO_X,TELA_MEIO_Y-50/2, ALLEGRO_ALIGN_CENTRE, "Pausado");
                al_flip_display();
                do {
                    c[0] = tela_tecla();
                } while(c[0]!='\n');
                tempoagora = relogio();
            }
            if (rodada != 3) confere_rodada(words,npalavras,&rodada,&rodada_acabou);
            tela_atualiza();
        }
    }
}

void testecalc() {
    float poligono[100];
    float poligono2[100];
    float angu;
    float angu2;
    while (true){
        angu2 = aleat(-1,361)*M_PI/180;
        for(int j = 0; j<350; j+=2){
            al_draw_filled_circle(TELA_MEIO_X,TELA_MEIO_Y,j,al_map_rgb(255,255,255));
            tela_atualiza();
        }
        for (int i = 0; i<360; i+=7){
            // for (int i = 0; i<361; i++){
            //     float posx = tela_rato_x();
            //     float posy = tela_rato_y();
            //     al_draw_circle(TELA_MEIO_X+cos(i*M_PI/180)*100,TELA_MEIO_Y+sin(i*M_PI/180)*100,10,al_map_rgb(255,255,255),1);
            //     al_draw_textf(tamanhosfontes[20],al_map_rgb(255,255,0),TELA_MEIO_X,TELA_MEIO_Y,ALLEGRO_ALIGN_CENTRE,"%f",atan2(posy-TELA_MEIO_Y,posx-TELA_MEIO_X));
            //     tela_atualiza();
            // }
            // al_draw_elliptical_arc(TELA_MEIO_X, TELA_MEIO_Y, 100, 100, M_PI/2, M_PI, al_map_rgb(250,0,0),10);
            // al_draw_elliptical_arc(TELA_MEIO_X, TELA_MEIO_Y, 100, 100, 3*M_PI/2, M_PI, al_map_rgb(250,255,0),10);
            angu = i*M_PI/180;
            float num = i;
            //angu2 = i<180 ? i*M_PI/180 : (360-i)*M_PI/180;
            al_calculate_arc(&poligono[0], 2 * sizeof(float), TELA_MEIO_X, TELA_MEIO_Y, 350 , 350, M_PI-angu/2, -(2*M_PI-angu), 0, 25);
            al_calculate_arc(&poligono[50], 2 * sizeof(float), poligono[0], TELA_MEIO_Y, ((poligono[24]-poligono[0])*(num/360)) ,(poligono[1]-poligono[49])/2, 3*M_PI/2, M_PI, 0, 25);
            for (int j = 0; j<100 ; j+=2){
                // al_draw_textf(tamanhosfontes[12],al_map_rgb(255,255-255*j/100,0),poligono[j],poligono[j+1],ALLEGRO_ALIGN_CENTRE,"x %.1f",poligono[j]);
                // al_draw_textf(tamanhosfontes[12],al_map_rgb(255,255-255*j/100,0),poligono[j],poligono[j+1]+20,ALLEGRO_ALIGN_CENTRE,"y %.1f",poligono[j+1]);
                // al_draw_textf(tamanhosfontes[12],al_map_rgb(255,255,255),poligono[j],poligono[j+1]-20,ALLEGRO_ALIGN_CENTRE,"%d",j);
                poligono2[j] = TELA_MEIO_X+(cos(angu2+atan2(poligono[j+1]-TELA_MEIO_Y,poligono[j]-TELA_MEIO_X)))*sqrt(pow(distancia_entre_pontos(poligono[j+1],TELA_MEIO_Y),2)+pow(distancia_entre_pontos(poligono[j],TELA_MEIO_X),2));
                poligono2[j+1] = TELA_MEIO_Y+(sin(angu2+atan2(poligono[j+1]-TELA_MEIO_Y,poligono[j]-TELA_MEIO_X)))*sqrt(pow(distancia_entre_pontos(poligono[j+1],TELA_MEIO_Y),2)+pow(distancia_entre_pontos(poligono[j],TELA_MEIO_X),2));
                //al_draw_circle(poligono[i],poligono[i+1],10,al_map_rgb(255,255-255*i/100,0),10);
            }
            al_draw_filled_polygon(poligono,50,al_map_rgb(255,255-255*(num/360),255-255*(num/360)));
            //al_draw_polygon(poligono2,50,ALLEGRO_LINE_JOIN_ROUND,al_map_rgb(0,0,255),5,2);
            tela_atualiza();
        }
    }
}

void desenhatroco();


void inicializa_matriz_palavras(char **v, char *nivel, int tam){
    FILE *arq = fopen(nivel,"r");
    char c;
    int j = 0;
    for (int i = 0; i < tam; ){
        c = fgetc(arq);
        if (c=='\n' || feof(arq)){
            v[i][j] = '\0';
            j = 0;
            i++;
        } else {
            v[i][j] = c;
            j++;
        }
    }
    fclose(arq);
}

int main (){
    srand(time(0));
    tela_inicio(TELA_LARGURA,TELA_ALTURA,"display");
    botao facil={TELA_MEIO_X-100,TELA_MEIO_Y-40,TELA_MEIO_X+100,TELA_MEIO_Y+40};
    botao medio={TELA_MEIO_X-100,TELA_MEIO_Y+60,TELA_MEIO_X+100,TELA_MEIO_Y+140};
    botao dificil={TELA_MEIO_X-100,TELA_MEIO_Y+160,TELA_MEIO_X+100,TELA_MEIO_Y+240};
    int linhasfacil = 94, linhasmedio = 29958, linhasdificil = 215314;
    char **vfacil = (char**) malloc(sizeof(char*)*linhasfacil);
    for (int i = 0; i<linhasfacil ; i++){
        vfacil[i] = (char*) malloc(sizeof(char)*24);
    }
    char **vmedio = (char**) malloc(sizeof(char*)*linhasmedio);
    for (int i = 0; i<linhasmedio ; i++){
        vmedio[i] = (char*) malloc(sizeof(char)*24);
    }
    char **vdificil = (char**) malloc(sizeof(char*)*linhasdificil);
    for (int i = 0; i<linhasdificil ; i++){
        vdificil[i] = (char*) malloc(sizeof(char)*24);
    }
    tamanhosfontes = (ALLEGRO_FONT**) malloc(sizeof(ALLEGRO_FONT*)*200);
    for (int i = 0; i<200; i++){
        tamanhosfontes[i] = al_load_font("pixelmix.ttf", i, 0);
    }
    inicializa_matriz_palavras(vfacil,"1",linhasfacil);
    inicializa_matriz_palavras(vmedio,"2",linhasmedio);
    inicializa_matriz_palavras(vdificil,"3",linhasdificil);
    while(true){
        al_draw_text(tamanhosfontes[100], al_map_rgb(255,68,255), TELA_MEIO_X, 200-100/2, ALLEGRO_ALIGN_CENTRE, "WorDestroy");
        desenha_botao(facil,10,verde,branco,"Facil",preto,20);
        desenha_botao(medio,10,amarelo,branco,"Medio",preto,20);
        desenha_botao(dificil,10,vermelho,branco,"Dificil",preto,20);
        if(tela_rato_clicado()){
            if(clicou_botao(facil)) partida(vfacil,linhasfacil);
            if(clicou_botao(medio)) partida(vmedio,linhasmedio);
            if(clicou_botao(dificil)) partida(vdificil,linhasdificil);
        }
        desenha_mouse();
        //testecalc();
        //desenhatroco();
        tela_atualiza();
    }
    tela_fim();
}





// a seguir sao funcoes que nao tem nada a ver com o jogo
// bem, de certa forma elas tem sim, sao prototipos
// que eu implementei de outro jeito no jogo em si
// mas elas sao so extras que eu fiquei com do de apagar, entao
// por conveniencia deixei ai como extras




void desenhatroco(){
    float ribbon[200];
    float trajeto[10];
    float angulomousearco;
    while(true){
        bool destruida = false;
        float angulo = atan2(aleat(0,TELA_ALTURA)-TELA_MEIO_Y,aleat(0,TELA_LARGURA)-TELA_MEIO_X);
        float raio = 1111;
        ponto circulo;
        float raioinimigo = 10;
        ponto central;
        central.x = TELA_MEIO_X;
        central.y = TELA_MEIO_Y;
        float raiojogador = 100;
        float inicioarco;
        float fimarco;
        float iniarcomousx;
        float iniarcomousy;
        float fimarcomousx;
        float fimarcomousy;
        float iniarcoangulo;
        float fimarcoangulo;
                trajeto[2] = aleat(0,3) == 1 ? 0 : TELA_LARGURA;
                trajeto[3] = aleat(0,3) == 1 ? 0 : TELA_ALTURA;
                for (int i = 4; i<6; i+=2){
                    trajeto[i] = aleat (0,TELA_LARGURA);
                    trajeto[i+1] = aleat (0,TELA_ALTURA);
                }
                trajeto[6] = TELA_MEIO_X+cos(angulo)*raiojogador;
                trajeto[7] = TELA_MEIO_Y+sin(angulo)*raiojogador*10;
                trajeto[8] = TELA_MEIO_X;
                trajeto[9] = TELA_MEIO_Y;
                al_calculate_spline(&ribbon[0], 2*sizeof(float), &trajeto[2], 0, 100);
        while (true){
            float posx = tela_rato_x();
            float posy = tela_rato_y();
            angulomousearco = atan2(posy-TELA_MEIO_Y,posx-TELA_MEIO_X);
            inicioarco = angulomousearco-(M_PI/2);
            fimarco = M_PI;
            iniarcomousx = TELA_MEIO_X + cos(angulomousearco-M_PI/2)*raiojogador;
            iniarcomousy = TELA_MEIO_Y + sin(angulomousearco-M_PI/2)*raiojogador;
            fimarcomousx = TELA_MEIO_X + cos(angulomousearco+M_PI/2)*raiojogador;
            fimarcomousy = TELA_MEIO_Y + sin(angulomousearco+M_PI/2)*raiojogador;
            iniarcoangulo = atan2(iniarcomousy-TELA_MEIO_Y,iniarcomousx-TELA_MEIO_X);
            fimarcoangulo = atan2(fimarcomousy-TELA_MEIO_Y,fimarcomousx-TELA_MEIO_X);
            al_draw_arc(TELA_MEIO_X,TELA_MEIO_Y,raiojogador,inicioarco,fimarco,al_map_rgb(255,255,255),5);
            if(posx<TELA_MEIO_X) {
                al_draw_arc(TELA_MEIO_X,TELA_MEIO_Y,raiojogador,M_PI,fimarcoangulo,al_map_rgb(0,255,50),5);
                al_draw_arc(TELA_MEIO_X,TELA_MEIO_Y,raiojogador,M_PI,iniarcoangulo,al_map_rgb(0,50,255),5);
            }
            circulo.x = ribbon[(int)(0+98*((1111-raio)/1111)+(int)(98*((1111-raio)/1111))%2)];
            circulo.y = ribbon[(int)(1+98*((1111-raio)/1111)+(int)(98*((1111-raio)/1111))%2)];
            //al_draw_circle(TELA_MEIO_X,TELA_MEIO_Y,raio,al_map_rgb(255,255,255),1);
            al_draw_circle(iniarcomousx,iniarcomousy,10,al_map_rgb(255,255,255),1);
            al_draw_circle(fimarcomousx,fimarcomousy,10,al_map_rgb(255,255,255),1);
            al_draw_textf(tamanhosfontes[12],al_map_rgb(0,255,255),TELA_MEIO_X,TELA_MEIO_Y,ALLEGRO_ALIGN_CENTRE,"angulo mouse: %.2f",angulomousearco*180/M_PI);
            al_draw_textf(tamanhosfontes[12],al_map_rgb(255,255,0),TELA_MEIO_X,TELA_MEIO_Y+200,ALLEGRO_ALIGN_CENTRE,"angulo bolinha: %.2f",angulo*180/M_PI);
            al_draw_textf(tamanhosfontes[18],al_map_rgb(255,255,255),iniarcomousx-10,iniarcomousy,ALLEGRO_ALIGN_CENTRE,"%.2f",iniarcoangulo*180/M_PI);
            al_draw_textf(tamanhosfontes[18],al_map_rgb(255,255,255),fimarcomousx-10,fimarcomousy,ALLEGRO_ALIGN_CENTRE,"%.2f",fimarcoangulo*180/M_PI);
            if(posx>TELA_MEIO_X){
                if (hipotenusa(circulo,central)<(raioinimigo+raiojogador) && angulo>inicioarco && angulo<(inicioarco+fimarco)) destruida = true;
            } else if (hipotenusa(circulo,central)<(raioinimigo+raiojogador) && ((angulo>-M_PI && angulo<fimarcoangulo) || (angulo<M_PI && angulo>iniarcoangulo))) destruida = true;
            if (!destruida){
                al_draw_circle(circulo.x,circulo.y,10,al_map_rgb(255,50,50),10);
            }
            al_draw_spline(&trajeto[2],al_map_rgb(255,255,255),5);
            tela_atualiza();
            raio-=5;
            if (raio <= -1110 || destruida){
                break;
            }
        }
    }
}





// typedef struct{
//     float x;
//     float y;
// } coordenada;

// typedef struct{
//     float velocidade;
//     float angulo;
//     float distancia;
//     ALLEGRO_COLOR cor;
//     coordenada pos;
//     bool naTela;
// } bolinha; 

// bola vetordebolas[5+nivel]
//     gerabolas;


// typedef struct{

// } jogo;

