#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum coloracao { Vermelho, Preto };
typedef enum coloracao Cor;

typedef struct no {
    struct no* pai;
    struct no* esquerda;
    struct no* direita;
    Cor cor;
    int valor;
} No;

typedef struct arvore {
    struct no* raiz;
    struct no* nulo; 
} Arvore;

//Altera a quantidade de números a serem inseridos na árvore
#define TAMANHO_TESTE 1000

int vazia(Arvore*);
void balancear(Arvore*, No*);
void rotacionarEsquerda(Arvore*, No*);
void rotacionarDireita(Arvore*, No*);
Arvore* criar();
No* criarNo(Arvore*, No*, int);
No* adicionar(Arvore*, int);
No* localizar(Arvore* arvore, int valor);
No* minimoArvore(Arvore* arvore, No *no);
void transplante(Arvore* arvore, No* u, No* v);
void corrige_remover(Arvore* arvore, No* no);
void remover(Arvore* arvore, No* y);

int iteracoes = 0;
int iteracoes_total = 0;

int main() {
    Arvore* a = criar();
    No* remover1 = NULL;
    int i = 0, inserir_numeros = 0, exec = 0;
    int v[TAMANHO_TESTE] = {0};
    int inseridos[10000] = {0};
    srand(time(0));

    for(exec = 0; exec < 10; exec++) { 
        a = criar();
        iteracoes = 0;

        for(i = 0; i < 10000; i++)
            inseridos[i] = 0;

        for(inserir_numeros = 0; inserir_numeros < TAMANHO_TESTE; inserir_numeros++) {
            do {
                i = rand() % 10000; 
            }while(inseridos[i]); 

            inseridos[i] = 1; 
            v[inserir_numeros] = i;      
            adicionar(a, i);
        }

        for(inserir_numeros = 0; inserir_numeros < TAMANHO_TESTE; inserir_numeros++) {
            No* remover_no = localizar(a, v[inserir_numeros]);

            if(remover_no != NULL && remover_no != a->nulo)
                remover(a, remover_no);
        }
        
        iteracoes_total += iteracoes;
        printf("Número de iterações da remoção %i: %i\n", exec + 1, iteracoes);
    }

   iteracoes_total /= 10;
   printf("Iterações médias de remoção de %i elementos: %i iterações\n", TAMANHO_TESTE, iteracoes_total);

   return 0;
}


Arvore* criar() {
    Arvore *arvore = malloc(sizeof(Arvore));
    arvore->nulo = NULL;
    arvore->raiz = NULL;

    arvore->nulo = criarNo(arvore, NULL, 0);
    arvore->nulo->cor = Preto;

    return arvore;
}


int vazia(Arvore* arvore) {
    return arvore->raiz == NULL;
}


No* criarNo(Arvore* arvore, No* pai, int valor) {
    No* no = malloc(sizeof(No));

    no->pai = pai;    
    no->valor = valor;
    no->direita = arvore->nulo;
    no->esquerda = arvore->nulo;

    return no;
}


No* adicionarNo(Arvore* arvore, No* no, int valor) {
    if (valor > no->valor) {
        if (no->direita == arvore->nulo) {
            no->direita = criarNo(arvore, no, valor);     
            no->direita->cor = Vermelho;       
        		
            return no->direita;
        } else {
            return adicionarNo(arvore, no->direita, valor);
        }
    } else {
        if (no->esquerda == arvore->nulo) {
            no->esquerda = criarNo(arvore, no, valor);
            no->esquerda->cor = Vermelho;
            
            return no->esquerda;
        } else {
            return adicionarNo(arvore, no->esquerda, valor);
        }
    }
}


No* adicionar(Arvore* arvore, int valor) {
    if (vazia(arvore)) {
        arvore->raiz = criarNo(arvore, arvore->nulo, valor);
        arvore->raiz->cor = Preto;
        
        return arvore->raiz;
    } else {
        No* no = adicionarNo(arvore, arvore->raiz, valor);
        balancear(arvore, no);

        return no;
    }
}


No* localizar(Arvore* arvore, int valor) {
    if (!vazia(arvore)) {
        No* no = arvore->raiz;

        while (no != arvore->nulo) {
            if (no->valor == valor) {
                return no;
            } else {
                no = valor < no->valor ? no->esquerda : no->direita;
            }
        }
    }

    return NULL;
}


void percorrerProfundidadeInOrder(Arvore* arvore, No* no, void (*callback)(int)) {
    if (no != arvore->nulo) {
        percorrerProfundidadeInOrder(arvore, no->esquerda,callback);
        callback(no->valor);
        percorrerProfundidadeInOrder(arvore, no->direita,callback);
    }
}


void percorrerProfundidadePreOrder(Arvore* arvore, No* no, void (*callback)(int)) {
    if (no != arvore->nulo) {
        callback(no->valor);
        percorrerProfundidadePreOrder(arvore, no->esquerda,callback);
        percorrerProfundidadePreOrder(arvore, no->direita,callback);
    }
}


void percorrerProfundidadePosOrder(Arvore* arvore, No* no, void (callback)(int)) {
    if (no != arvore->nulo) {
        percorrerProfundidadePosOrder(arvore, no->esquerda,callback);
        percorrerProfundidadePosOrder(arvore, no->direita,callback);
        callback(no->valor);
    }
}


void visitar(int valor) {
    printf("%d ", valor);
}


void balancear(Arvore* arvore, No* no) {
    (iteracoes)++;
    while (no->pai->cor == Vermelho) {
        if (no->pai == no->pai->pai->esquerda) {
            No *tio = no->pai->pai->direita;
            
            if (tio->cor == Vermelho) {
                tio->cor = Preto; //Caso 1
                no->pai->cor = Preto; 

                no->pai->pai->cor = Vermelho; //Caso 1
                no = no->pai->pai; //Caso 1
            } else {
                if (no == no->pai->direita) {
                    no = no->pai; //Caso 2
                    rotacionarEsquerda(arvore, no); //Caso 2
                } else {
                    no->pai->cor = Preto; 
                    no->pai->pai->cor = Vermelho; //Caso 3
                    rotacionarDireita(arvore, no->pai->pai); //Caso 3
                }
            }
        } else {
            No *tio = no->pai->pai->esquerda;
            
            if (tio->cor == Vermelho) {
                tio->cor = Preto; //Caso 1
                no->pai->cor = Preto; 

                no->pai->pai->cor = Vermelho; //Caso 1
                no = no->pai->pai; //Caso 1
            } else {
                if (no == no->pai->esquerda) {
                    no = no->pai; //Caso 2
                    rotacionarDireita(arvore, no); //Caso 2
                } else {
                    no->pai->cor = Preto; 
                    no->pai->pai->cor = Vermelho; //Caso 3
                    rotacionarEsquerda(arvore, no->pai->pai); //Caso 3
                }
            }
        }
    }
    arvore->raiz->cor = Preto; //Conserta possível quebra regra 2
}


void rotacionarEsquerda(Arvore* arvore, No* no) {
    No* direita = no->direita;
    no->direita = direita->esquerda; 

    if (direita->esquerda != arvore->nulo) {
        direita->esquerda->pai = no;
    }

    direita->pai = no->pai;
    
    if (no->pai == arvore->nulo) {
        arvore->raiz = direita;
    } else if (no == no->pai->esquerda) {
        no->pai->esquerda = direita;
    } else {
        no->pai->direita = direita;
    }

    direita->esquerda = no;
    no->pai = direita;
}


void rotacionarDireita(Arvore* arvore, No* no) {
    No* esquerda = no->esquerda;
    no->esquerda = esquerda->direita;
    
    if (esquerda->direita != arvore->nulo) {
        esquerda->direita->pai = no;
    }
    
    esquerda->pai = no->pai;
    
    if (no->pai == arvore->nulo) {
        arvore->raiz = esquerda;
    } else if (no == no->pai->esquerda) {
        no->pai->esquerda = esquerda;
    } else {
        no->pai->direita = esquerda;
    }
    
    esquerda->direita = no;
    no->pai = esquerda;
}


void corrige_remover(Arvore* arvore, No* no) {
    while(no != arvore->raiz && no->cor == Preto){
        (iteracoes)++;

        if(no == no->pai->esquerda) {
            No* w = no->pai->direita;

            if(w->cor == Vermelho) {
                w->cor = Preto;
                no->pai->cor = Vermelho;
                rotacionarEsquerda(arvore, no->pai);
                w = no->pai->direita;
            }

            if(w->esquerda->cor == Preto && w->direita->cor == Preto) {
                w->cor = Vermelho;
                no = no->pai;
            } 

            else {
                if(w->direita->cor == Preto) {
                    w->esquerda->cor = Preto;
                    w->cor = Vermelho;
                    rotacionarDireita(arvore, w);
                    w = no->pai->direita;
                }

                w->cor = no->pai->cor;
                no->pai->cor = Preto;
                w->direita->cor = Preto;
                rotacionarEsquerda(arvore, no->pai);
                no = arvore->raiz;
            }
        } 
        else {
            No* w = no->pai->esquerda;

            if(w->cor == Vermelho) {
                w->cor = Preto;
                no->pai->cor = Vermelho;
                rotacionarDireita(arvore, no->pai);
                w = no->pai->esquerda;
            }

            if(w->direita->cor == Preto && w->esquerda->cor == Preto) {
                w->cor = Vermelho;
                no = no->pai;
            } 

            else {
                if(w->esquerda->cor == Preto) {
                    w->direita->cor = Preto;
                    w->cor = Vermelho;
                    rotacionarEsquerda(arvore, w);
                    w = no->pai->esquerda;
                }

                w->cor = no->pai->cor;
                no->pai->cor = Preto;
                w->esquerda->cor = Preto;
                rotacionarDireita(arvore, no->pai);
                no = arvore->raiz;
            }
        }
    }
    no->cor = Preto;
}


void transplante(Arvore* arvore, No* u, No* v) {
    (iteracoes++);
    if(u->pai == arvore->nulo)
        arvore->raiz = v;

    else if(u == u->pai->esquerda)
        u->pai->esquerda = v;

    else
        u->pai->direita = v;

    v->pai = u->pai;
}


void remover(Arvore* arvore, No* z) {
    (iteracoes++);
    No* y = z;
    No* no = NULL;
    Cor y_cor_original = y->cor;

    if(z->esquerda == arvore->nulo) {
        transplante(arvore, z, z->direita);
        no = z->direita;
    }

    else if(z->direita == arvore->nulo) {
        transplante(arvore, z, z->esquerda);
        no = z->esquerda;
    }

    else {
        y = minimoArvore(arvore, z->direita);
        y_cor_original = y->cor;
        no = y->direita;

        if(y != z->direita) {
            transplante(arvore, y, y->direita);
            y->direita = z->direita;
            y->direita->pai = y;
        }

        else
            no->pai = y;

        transplante(arvore, z, y);
        y->esquerda = z->esquerda;
        y->esquerda->pai = y;
        y->cor = z->cor;
    }

    if(y_cor_original == Preto)
        corrige_remover(arvore, no);
}


No* minimoArvore(Arvore* arvore, No *no) {
    while (no->esquerda != arvore->nulo) 
        no = no->esquerda;

    return no;
}