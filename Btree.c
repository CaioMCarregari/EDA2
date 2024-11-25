#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


#define MAX 3 // Grau máximo da árvore
#define MIN (MAX + 1) / 2 // Grau mínimo de nós internos

// Estrutura de um nó da árvore B
typedef struct noB {
    int chaves[MAX];
    int total; // Número de chaves no nó
    struct noB* filhos[MAX + 1];
    bool isLeaf;
} No;

// Cria um novo nó
No* criaNo(bool isLeaf) {
    No* no = (No*)malloc(sizeof(No));
    no->total = 0;
    no->isLeaf = isLeaf;
    for (int i = 0; i <= MAX; i++) {
        no->filhos[i] = NULL;
    }
    return no;
}

// Exibe a árvore (modo hierárquico)
void mostraArv(No* raiz, int level) {
    if (raiz) {
        for (int i = raiz->total - 1; i >= 0; i--) {
            mostraArv(raiz->filhos[i + 1], level + 1);
            for (int j = 0; j < level; j++) {
                printf("   ");
            }
            printf("%d\n", raiz->chaves[i]);
        }
        mostraArv(raiz->filhos[0], level + 1);
    }
}

// Divide um nó cheio
void split(No* pai, int index, No* filho, int *contador) {
    No* novo = criaNo(filho->isLeaf);
    novo->total = MIN - 1;

    // Move as chaves e filhos para o novo nó
    for (int i = 0; i < MIN - 1; i++) {
        novo->chaves[i] = filho->chaves[i + MIN];
    }
    (*contador)++;

    if (!filho->isLeaf) {
        for (int i = 0; i < MIN; i++) {
            novo->filhos[i] = filho->filhos[i + MIN];
        }
        (*contador)++;
    }

    filho->total = MIN - 1;

    for (int i = pai->total; i >= index + 1; i--) {
        pai->filhos[i + 1] = pai->filhos[i];
    }
    (*contador)++;

    pai->filhos[index + 1] = novo;

    for (int i = pai->total - 1; i >= index; i--) {
        pai->chaves[i + 1] = pai->chaves[i];
    }
    (*contador)++;
    pai->chaves[index] = filho->chaves[MIN - 1];
    pai->total++;
}


void insertNaoCheio(No* no, int chave, int *contador) {
    int i = no->total - 1;

    if (no->isLeaf) {
        while (i >= 0 && chave < no->chaves[i]) {
            no->chaves[i + 1] = no->chaves[i];
            i--;
        }
        no->chaves[i + 1] = chave;
        no->total++;
        (*contador)++;
    } else {
        while (i >= 0 && chave < no->chaves[i]) {
            i--;
        }
        (*contador)++;
        i++;
        if (no->filhos[i]->total == MAX) {
            split(no, i, no->filhos[i], contador);
            if (chave > no->chaves[i]) {
                i++;
            }
            (*contador)++;
        }
        insertNaoCheio(no->filhos[i], chave, contador);
        (*contador)++;
    }
}


No* insert(No* raiz, int chave, int *contador) {
    if (!raiz) {
        raiz = criaNo(true);
        raiz->chaves[0] = chave;
        raiz->total = 1;
        (*contador)++;
        return raiz;
    }

    if (raiz->total == MAX) {
        No* novo = criaNo(false);
        novo->filhos[0] = raiz;
        split(novo, 0, raiz, contador);
        int i = (chave > novo->chaves[0]) ? 1 : 0;
        insertNaoCheio(novo->filhos[i], chave, contador);
        (*contador)++;
        return novo;
    }

    insertNaoCheio(raiz, chave, contador);
    (*contador)++;
    return raiz;
}

void removeChaveNo(No* no, int index, int *contador) {
    for (int i = index; i < no->total - 1; i++) {
        no->chaves[i] = no->chaves[i + 1];
    }
    no->total--;
    (*contador)++;
}

int achaPredecessor(No* no, int *contador) {
    while (!no->isLeaf) {
        no = no->filhos[no->total];
    }
    (*contador)++;
    return no->chaves[no->total - 1];
}


int achaSucessor(No* no, int *contador) {
    while (!no->isLeaf) {
        no = no->filhos[0];
    }
    (*contador)++;
    return no->chaves[0];
}

void mergeNo(No* pai, int index, int *contador) {
    No* filho = pai->filhos[index];
    No* irmao = pai->filhos[index + 1];

    filho->chaves[filho->total] = pai->chaves[index];

    for (int i = 0; i < irmao->total; i++) {
        filho->chaves[filho->total + 1 + i] = irmao->chaves[i];
    }
    (*contador)++;

    if (!filho->isLeaf) {
        for (int i = 0; i <= irmao->total; i++) {
            filho->filhos[filho->total + 1 + i] = irmao->filhos[i];
        }
        (*contador)++;
    }

    for (int i = index; i < pai->total - 1; i++) {
        pai->chaves[i] = pai->chaves[i + 1];
    }
    (*contador)++;

    for (int i = index + 1; i < pai->total; i++) {
        pai->filhos[i] = pai->filhos[i + 1];
    }
    (*contador)++;

    filho->total += irmao->total + 1;
    pai->total--;

    free(irmao);
    (*contador)++;
}

// Remove uma chave da árvore
No* removeChave(No* raiz, int chave, int *contador) {
    if (!raiz) {
        printf("A árvore está vazia.\n");
        return NULL;
    }

    int index = 0;
    while (index < raiz->total && chave > raiz->chaves[index]) {
        index++;
    }
    (*contador)++;

    if (index < raiz->total && chave == raiz->chaves[index]) {
        if (raiz->isLeaf) {
            removeChaveNo(raiz, index, contador);
            (*contador)++;
        } else if (raiz->filhos[index]->total >= MIN) {
            int predecessor = achaPredecessor(raiz->filhos[index], contador);
            raiz->chaves[index] = predecessor;
            raiz->filhos[index] = removeChave(raiz->filhos[index], predecessor, contador);
            (*contador)++;
        } else if (raiz->filhos[index + 1]->total >= MIN) {
            int successor = achaSucessor(raiz->filhos[index + 1], contador);
            raiz->chaves[index] = successor;
            raiz->filhos[index + 1] = removeChave(raiz->filhos[index + 1], successor, contador);
            (*contador)++;
        } else {
            mergeNo(raiz, index, contador);
            raiz = removeChave(raiz->filhos[index], chave, contador);
            (*contador)++;
        }
        (*contador)++;
    } else if (!raiz->isLeaf) {
        if (raiz->filhos[index]->total < MIN) {
            if (index > 0 && raiz->filhos[index - 1]->total >= MIN) {
                No* filho = raiz->filhos[index];
                No* irmao = raiz->filhos[index - 1];

                for (int i = filho->total - 1; i >= 0; i--) {
                    filho->chaves[i + 1] = filho->chaves[i];
                }
                (*contador)++;

                if (!filho->isLeaf) {
                    for (int i = filho->total; i >= 0; i--) {
                        filho->filhos[i + 1] = filho->filhos[i];
                    }
                }
                (*contador)++;

                filho->chaves[0] = raiz->chaves[index - 1];

                if (!irmao->isLeaf) {
                    filho->filhos[0] = irmao->filhos[irmao->total];
                }
                (*contador)++;

                raiz->chaves[index - 1] = irmao->chaves[irmao->total - 1];
                filho->total++;
                irmao->total--;
                (*contador)++;
            } else if (index < raiz->total && raiz->filhos[index + 1]->total >= MIN) {
                No* filho = raiz->filhos[index];
                No* irmao = raiz->filhos[index + 1];

                filho->chaves[filho->total] = raiz->chaves[index];
                raiz->chaves[index] = irmao->chaves[0];

                if (!filho->isLeaf) {
                    filho->filhos[filho->total + 1] = irmao->filhos[0];
                }

                for (int i = 0; i < irmao->total - 1; i++) {
                    irmao->chaves[i] = irmao->chaves[i + 1];
                }

                if (!irmao->isLeaf) {
                    for (int i = 0; i < irmao->total; i++) {
                        irmao->filhos[i] = irmao->filhos[i + 1];
                    }
                }

                filho->total++;
                irmao->total--;
                (*contador)++;
            } else {
                if (index < raiz->total) {
                    mergeNo(raiz, index, contador);
                } else {
                    mergeNo(raiz, index - 1, contador);
                }
            }
            (*contador)++;
        }

        raiz->filhos[index] = removeChave(raiz->filhos[index], chave, contador);
    }

    if (raiz->total == 0) {
        No* temp = raiz;
        if (raiz->isLeaf) {
            raiz = NULL;
        } else {
            raiz = raiz->filhos[0];
        }
        free(temp);
    }
    (*contador)++;
    return raiz;
}

int buscaSequencial(int arr[], int tamanho, int alvo) {
    for (int i = 0; i < tamanho; i++) {
        if (arr[i] == alvo) {
            return i;
        }
    }
    return -1; 
}


int main() {
    srand(time(NULL));
    No* raiz = NULL;
    int entrada = 10;
    int inserir[entrada];
    int deletar[entrada/2];
    int a;
    int contador = 0;

    for (int i = 0; i < entrada; i++){
        a = rand() % 10000;
        if (buscaSequencial(inserir, i, a) != -1){
            i--;
            continue;
        }
        inserir[i] = a;
    }

    for (int i = 0; i < entrada; i++){
        raiz = insert(raiz, inserir[i], &contador);
    }

    printf("\nContador->inserir: %i", contador);
    // mostraArv(raiz, 0);



    contador = 0;
    printf("\n----------------------");



    for (int i = 0; i < entrada; i++){
        a = rand() % entrada;
        if (buscaSequencial(deletar, i, a) != -1){
            i--;
            continue;
        }
        deletar[i] = a;
    }
    for (int i = 0; i < entrada/2; i++){
        raiz = removeChave(raiz, inserir[deletar[i]], &contador);
    }
    

    printf("\nContador->deletar: %i\n", contador);
    // mostraArv(raiz, 0);

    return 0;
}
