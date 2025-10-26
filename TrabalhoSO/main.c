#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#pragma comment(lib,"pthreadVC2.lib")

#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


#define LINHAS        5000  //Linhas matriz
#define COLUNAS       5000  //Colunas matriz
#define SUB_LINHAS    250   //linhas sub-matriz
#define SUB_COLUNAS   100   //Colunas sub-matriz
#define SEMENTE       1233  //Semente para gerar numeros aleatórios
#define QUANT_THREADS 2     //Define Quantidade das Threads criadas


int quantidade_sub_matrizes = 0, contador = 0;
int** matriz = NULL;

int qtd_submatrizes(int i, int j, int sub_i, int sub_j) {
    return ((i - sub_i + 1) * (j - sub_j + 1));
}

// ALOCAR ESPAÇO PARA MATRIZ
int** alocarMatriz() {
    int i, j;

    // ALOCAR ESPAÇO PARA LINHAS
    matriz = calloc(LINHAS, sizeof(int*));
    if (matriz == NULL) {
        printf("Erro ao alocar memória.\n");
        return (NULL);
    }

    // ALOCAR ESPAÇO PARA COLUNAS
    for (i = 0; i < LINHAS; i++) {
        matriz[i] = (int*)malloc(COLUNAS * sizeof(int));
        if (matriz[i] == NULL) {
            printf("Erro ao alocar memória.\n");
            return (NULL);
        }
    }

    return matriz;
}

// LIBERAR MEMÓRIA ALOCADA
void liberarMatriz() {
    for (int i = 0; i < LINHAS; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

// RETORNAR SE UM NÚMERO É OU NÃO PRIMO
int ehPrimo(int numero) {
    if (numero <= 1) { return 0; }
    int raiz = (int)sqrt(numero);

    for (int i = 2; i <= raiz; i++) {
        if (numero % i == 0) { return 0; }
    }

    return 1;
}

//BUSCA SERIAL
void buscaSerial() {

    matriz = alocarMatriz();
    clock_t inicio, fim;
    double tempo_gasto;
    inicio = clock();

    srand(SEMENTE);
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            matriz[i][j] = rand() % 32000;
        }
    }

    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            contador += ehPrimo(matriz[i][j]);
        }
    }
    fim = clock();
    tempo_gasto = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

    printf("Tempo de execução busca serial: %.3f segundos\n", tempo_gasto);
    printf("\nQuantidade de primos: %d\n", contador);

    liberarMatriz(matriz, LINHAS);

}

void* funcaoThreadTeste(void* arg) {

	pthread_t idThread = pthread_self();
    printf("Thread ID: %p\n", (void*)&idThread);
    return NULL;
}

void buscaParalela() {

    int arrayPos;
    //CRIAR THREADS
	pthread_t arrayThreads[QUANT_THREADS];
    
    for (arrayPos = 0; arrayPos < QUANT_THREADS; arrayPos++) {
        pthread_create(&arrayThreads[arrayPos], NULL, funcaoThreadTeste, NULL);
    }
    for (arrayPos = 0; arrayPos < QUANT_THREADS; arrayPos++) {
        pthread_join(arrayThreads[arrayPos], NULL);
    }

}

// FUNÇÃO PRINCIPAL
int main() {

    buscaSerial();
    buscaParalela();

    return 0;

}