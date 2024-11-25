#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct no {
    int valor;
    struct no *esq;
    struct no *dir;
    int altura;
} No;

//Altera a quantidade de números a serem inseridos na árvore
#define TAMANHO_TESTE 1000

No* novoNo(int x);
No* rotacaoEsq(No *r);
No* rotacaoDir(No *r);
No* rotacaoDirEsq(No *r);
No* rotacaoEsqDir(No *r);
No* inserir(No *raiz, int x);
No* balancear(No *raiz);
No* remover(No *raiz, int x);
int maior(int a, int b);
int alturaDoNo(No *no);
int balanceamento(No *no);

int iteracoes = 0;
int iteracoes_total = 0;

int main() {
    No *raiz = NULL;
    int i = 0, inserir_numeros = 0, exec = 0;
    int v[TAMANHO_TESTE] = {0};
    int inseridos[10000] = {0};
    srand(time(0));

    for(exec = 0; exec < 10; exec++) { 
        raiz = NULL; 
        iteracoes = 0;

        for(i = 0; i < 10000; i++)
            inseridos[i] = 0;

        for(inserir_numeros = 0; inserir_numeros < TAMANHO_TESTE; inserir_numeros++) {
            do {
                i = rand() % 10000; 
            }while(inseridos[i]); 

            inseridos[i] = 1;       
            v[inserir_numeros] = i;
            raiz = inserir(raiz, i);
        }

        for(inserir_numeros = 0; inserir_numeros < TAMANHO_TESTE; inserir_numeros++)
            raiz = remover(raiz, v[inserir_numeros]);

        iteracoes_total += iteracoes;
        printf("Número de iterações da remoção %i: %i\n", exec + 1, iteracoes);
    }

    iteracoes_total /= 10;
    printf("Iterações médias de remoção de %i elementos: %i iterações\n", TAMANHO_TESTE, iteracoes_total);

    return 0;
}


No* novoNo(int x) {
    No *novo = malloc(sizeof(No));

    if(novo) {
        novo->valor = x;
        novo->esq = NULL;
        novo->dir = NULL;
        novo->altura = 0;
    }
    else
        printf("Erro ao alocar memória!\n");
    return novo;
}


int maior(int a, int b) {
    if(a > b)
        return a;
    else
        return b;
}


int alturaDoNo(No *no) {
    if(no == NULL)
        return -1;
    else
        return no->altura;
}


int balanceamento(No *no) {
    if(no) {
        (iteracoes)++;
        return (alturaDoNo(no->esq) - alturaDoNo(no->dir));
    }
    else
        return 0;
}


No* rotacaoEsq(No *r) {
    No *y, *f;
    
    y = r->dir;
    f = y->esq;

    y->esq = r;
    r->dir = f;

    r->altura = maior(alturaDoNo(r->esq), alturaDoNo(r->dir)) + 1;
    y->altura = maior(alturaDoNo(y->esq), alturaDoNo(y->dir)) + 1;

    return y;
}


No* rotacaoDir(No *r) {
    No *y, *f;

    y = r->esq;
    f = y->dir;

    y->dir = r;
    r->esq = f;

    r->altura = maior(alturaDoNo(r->esq), alturaDoNo(r->dir)) + 1;
    y->altura = maior(alturaDoNo(y->esq), alturaDoNo(y->dir)) + 1;

    return y;

}


No* rotacaoDirEsq(No *r) {
    r->dir = rotacaoDir(r->dir);
    return rotacaoEsq(r);
}


No* rotacaoEsqDir(No *r) {
    r->esq = rotacaoEsq(r->esq);
    return rotacaoDir(r);
}


No* balancear(No *raiz) {
    int fb = balanceamento(raiz);

    if(fb < -1 && balanceamento(raiz->dir) <= 0) {
        (iteracoes)++;
        raiz = rotacaoEsq(raiz);
    }

    else if(fb > 1 && balanceamento(raiz->esq) >=0) {
        (iteracoes)++;
        raiz = rotacaoDir(raiz);
    }

    else if(fb > 1 && balanceamento(raiz->esq) < 0) {
        (iteracoes)++;
        raiz = rotacaoEsqDir(raiz);
    }

    else if(fb < -1 && balanceamento(raiz->dir) > 0) {
        (iteracoes)++;
        raiz = rotacaoDirEsq(raiz);
    }

    return raiz;
}


No* inserir(No *raiz, int x) {
    if(raiz == NULL)
        return novoNo(x);

    else {
        if(x < raiz->valor)  
            raiz->esq = inserir(raiz->esq, x);

        else if(x > raiz->valor) 
            raiz->dir = inserir(raiz->dir, x);
    }
    raiz->altura = maior(alturaDoNo(raiz->esq), alturaDoNo(raiz->dir)) + 1;

    raiz = balancear(raiz);

    return raiz;
}


No* remover(No *raiz, int x) {
    if(raiz == NULL)
        return NULL;

    else {
        (iteracoes)++;
        if(raiz->valor == x) {
            if(raiz->esq == NULL && raiz->dir == NULL) {
                free(raiz);
                return NULL;
            }
            else {
                if(raiz->esq != NULL && raiz->dir != NULL) {
                    No *aux = raiz->esq;
                    while(aux->dir != NULL)
                        aux = aux->dir;
                    raiz->valor = aux->valor;
                    aux->valor = x;
                    raiz->esq = remover(raiz->esq, x);
                    return raiz;
                }
                else {
                    No *aux;
                    if(raiz->esq != NULL)
                        aux = raiz->esq;
                    else
                        aux = raiz->dir;
                    free(raiz);
                    return aux;
                }
            }
        }   
        else {
            if(x < raiz->valor)
                raiz->esq = remover(raiz->esq, x);
            else
                raiz->dir = remover(raiz->dir, x);
        }
        raiz->altura = maior(alturaDoNo(raiz->esq), alturaDoNo(raiz->dir)) + 1;
        
        raiz = balancear(raiz);
        return raiz; 
    }
}