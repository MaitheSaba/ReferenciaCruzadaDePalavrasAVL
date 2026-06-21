/*
DECLARAÇÃO DO USO DE IA
Claude.ai - versão Sonnet 4.6
 - Melhoria no método de inserir nodos (newNode):
 	- verificação de erro de alocação (if(!new)
	- uso de strncpy ao invés de strcpy (permite atribuição de um número específico de caracteres, evitando tentar alocar mais do que o struct Avl permite)

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct avl{
	char word[100];
	int numOc;
	int height;
	struct avl *left;
	struct avl *right;
} Avl;

typedef struct letter{
	char letter;
	Avl *avl;
	struct letter *next;
} Letter;

typedef struct cr{
	Letter *letters;
} CrossReference;

Avl *newNode(char *word){
	Avl *newNode = (Avl*) malloc(sizeof(Avl));
	if(!newNode){
		fprintf(stderr, "Erro de alocação!\n");
		exit(1);
	}
	strncpy(newNode->word, word, 99);
	newNode->word[99] = '\0';
	newNode->numOc = 1;
	newNode->height = 1;
	newNode->left = NULL;
	newNode->right = NULL;
	return newNode;
}

Letter *newLetter(CrossReference *cr, char letter){
	Letter *prev = NULL, *aux = cr->letters;
	
	while(aux && aux->letter < letter){
		prev = aux;
		aux = aux->next;
	}
	
	Letter *newLetter = (Letter *) malloc(sizeof(Letter));
	if(!newLetter){
		fprintf(stderr, "Erro de alocação\n");
		exit(1);
	}
	newLetter->letter = letter;
	newLetter->avl = NULL;
	newLetter->next = aux;
	
	if(!prev){
		cr->letters = newLetter;
	}
	else{
		prev->next = newLetter;
	}
	return newLetter;
}
//Funções AVL
int getHeight(Avl* n)
{
	return n == NULL ? 0 : n->height;
}

int max(int a, int b){
	return a > b ? a : b;	
}

int getBalanceFactor(Avl* n){
	return n == NULL ? 0 : (getHeight(n->left) - getHeight(n->right));
}

void updateHeight(Avl* n){
	if(n != NULL){
		n->height = 1 + max(getHeight(n->left), getHeight(n->right));
	}
}

Avl *rotateRight(Avl *y){
	Avl *x = y->left;
	Avl *t2 = x->right;
	
	x->right = y;
	y->left = t2;
	
	updateHeight(y);
	updateHeight(x);
	return x;
}

Avl *rotateLeft(Avl *x){
	Avl *y = x->right;
	Avl *t2 = y->left;

	y->left = x;	
	x->right = t2;
	
	updateHeight(x);
	updateHeight(y);
	return y;
}

Avl *balance(Avl *n){
	updateHeight(n);
	int balanceFactor = getBalanceFactor(n);
	
	if(balanceFactor > 1 && getBalanceFactor(n->left) >= 0){
		return rotateRight(n);
	}
	
	if(balanceFactor > 1 && getBalanceFactor(n->left) < 0){
		n->left = rotateLeft(n->left);
		return rotateRight(n);
	}
	
	if(balanceFactor < -1 && getBalanceFactor(n->right) <= 0){
		return rotateLeft(n);
	}
	
	if(balanceFactor < -1 && getBalanceFactor(n->right) > 0){
		n->right = rotateRight(n->right);
		return rotateLeft(n);
	}
	
	return n;
}
//

Avl *avlInsert(Avl *root, char *word, int *insert){
	if(!root){
		*insert = 1;
		return newNode(word);
	}
	int cmp = strcmp(word, root->word);
	if(cmp < 0){
		root->left = avlInsert(root->left, word, insert);
	} else if(cmp > 0){
		root->right = avlInsert(root->right, word, insert);
	} else{
		root->numOc++;
		*insert = 0;
		return root;
	}
	return balance(root);
}

Letter *getLetter(CrossReference *cr, char letter){
	Letter *prev = NULL, *aux = cr->letters;
	
	while(aux && aux->letter < letter){
		prev = aux;
		aux = aux->next;
	}
	
	if(aux && aux->letter == letter){
		return aux;
	}
	
	return NULL;
}


void insertText(CrossReference *cr, char *txt){
	char aux[100];
	int i = 0, j = 0;
	int total = strlen(txt);
	
	while(i <= total){
		char c = txt[i];
		if(isalpha((unsigned char)c)){
			if(j < 99){
				aux[j++] = tolower((unsigned char)c);
			}
		}	
		else{
			if(j > 0){
				aux[j] = '\0';
				char letter = aux[0];
				Letter *l = getLetter(cr, letter);
				if(!l){
					l = newLetter(cr, letter);
				}
				int insert = 0;
				l->avl = avlInsert(l->avl, aux, &insert);
				j = 0;
			}
		}
		i++;
	}
}

Avl *consultarPalavra(Avl *raiz, char *palavra){
    if(raiz == NULL)
        return NULL;

    int cmp = strcmp(palavra, raiz->word);

    if(cmp == 0)
        return raiz;

    if(cmp < 0)
        return consultarPalavra(raiz->left, palavra);

    return consultarPalavra(raiz->right, palavra);
}

int contarPalavras(Avl *raiz){
    if(raiz == NULL)
        return 0;

    return 1 + contarPalavras(raiz->left)
             + contarPalavras(raiz->right);
}

int contarOcorrencias(Avl *raiz){
    if(raiz == NULL)
        return 0;

    return raiz->numOc
         + contarOcorrencias(raiz->left)
         + contarOcorrencias(raiz->right);
}

void exibirEmOrdem(Avl *raiz){
    if(raiz){
        exibirEmOrdem(raiz->left);

        printf("%s - %d\n", raiz->word, raiz->numOc);

        exibirEmOrdem(raiz->right);
    }
}

void exibirEmOrdemInversa(Avl *raiz){
    if(raiz){
        exibirEmOrdemInversa(raiz->right);

        printf("%s - %d\n", raiz->word, raiz->numOc);

        exibirEmOrdemInversa(raiz->left);
    }
}

void exibirPreFixado(Avl *raiz){
    if(raiz){
        printf("%s\n", raiz->word);

        exibirPreFixado(raiz->left);
        exibirPreFixado(raiz->right);
    }
}

void buscarMaiorOcorrencia(Avl *raiz, int *maior){
    if(raiz){
        if(raiz->numOc > *maior)
            *maior = raiz->numOc;

        buscarMaiorOcorrencia(raiz->left, maior);
        buscarMaiorOcorrencia(raiz->right, maior);
    }
}

void exibirMaioresOcorrencias(Avl *raiz, int maior){
    if(raiz){
        exibirMaioresOcorrencias(raiz->left, maior);

        if(raiz->numOc == maior)
            printf("%s - %d\n", raiz->word, raiz->numOc);

        exibirMaioresOcorrencias(raiz->right, maior);
    }
}

void liberarAvl(Avl *raiz){
    if(raiz){
        liberarAvl(raiz->left);
        liberarAvl(raiz->right);

        free(raiz);
    }
}

void liberarLetras(Letter *letra){
    while(letra){
        Letter *aux = letra;

        liberarAvl(letra->avl);

        letra = letra->next;

        free(aux);
    }
}

void init(CrossReference *cr){
	cr->letters = NULL;
	
	char *txt = 
		"o menino viu o gato no muro o gato pulo o muro e foi para a rua a rua estava"
		"deserta e o gato correu rapido o menino correu atras do gato mas o gato"
		"sumiu na noite a noite estava escura e o menino sentiu medo o medo fez o"
		"menino voltar para a casa a casa estava quente e o pai do menino estava na"
		"sala o pai perguntou do gato e o menino disse que o gato fugiu para a rua"
		"entao o pai e o menino foram procurar o gato de novo no muro";
		
	insertText(cr, txt);
}

int menu(){
    int op;

    printf("\nEscolha uma opcao:\n");
    printf("1. Inserir palavra\n");
    printf("2. Consultar palavra\n");
    printf("3. Remover palavra\n");
    printf("4. Ver total de palavras\n");
    printf("5. Ver total de ocorrencias\n");
    printf("6. Ver palavras\n");
    printf("7. Ver palavras por letra\n");
    printf("8. Ver palavra com maior numero de ocorrencias\n");
    printf("9. Ver palavras em ordem pre-fixada\n");
    printf("0. Sair\n");

    scanf("%d", &op);

    return op;
}


int main(){
    CrossReference cr;
    init(&cr);

    int op;

    do{
        op = menu();

        switch(op){

            case 1:
                printf("Inserir palavra\n");
                break;

            case 2:
                printf("Consultar palavra\n");
                break;

            case 3:
                printf("Remover palavra\n");
                break;

            case 4:{
                int total = 0;
                Letter *aux = cr.letters;

                while(aux){
                    total += contarPalavras(aux->avl);
                    aux = aux->next;
                }

                printf("Total de palavras: %d\n", total);
                break;
            }

            case 5:{
                int total = 0;
                Letter *aux = cr.letters;

                while(aux){
                    total += contarOcorrencias(aux->avl);
                    aux = aux->next;
                }

                printf("Total de ocorrencias: %d\n", total);
                break;
            }

            case 6:{
                Letter *aux = cr.letters;

                while(aux){
                    exibirEmOrdem(aux->avl);
                    aux = aux->next;
                }

                break;
            }

            case 7:{
                char letra;

                printf("Digite a letra: ");
                scanf(" %c", &letra);

                letra = tolower(letra);

                Letter *l = getLetter(&cr, letra);

                if(l)
                    exibirEmOrdem(l->avl);
                else
                    printf("Nao existem palavras com essa letra.\n");

                break;
            }

            case 8:{
                int maior = 0;
                Letter *aux = cr.letters;

                while(aux){
                    buscarMaiorOcorrencia(aux->avl, &maior);
                    aux = aux->next;
                }

                printf("Palavras com maior numero de ocorrencias:\n");

                aux = cr.letters;

                while(aux){
                    exibirMaioresOcorrencias(aux->avl, maior);
                    aux = aux->next;
                }

                break;
            }

            case 9:{
                Letter *aux = cr.letters;

                while(aux){
                    exibirPreFixado(aux->avl);
                    aux = aux->next;
                }

                break;
            }

            case 10:
                break;

            case 0:
                printf("Saindo...\n");
                break;

            default:
                printf("Opcao invalida.\n");
        }

    }while(op != 0);

    liberarLetras(cr.letters);

    return 0;
}
