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


#define LINHAS        10000 //Linhas matriz
#define COLUNAS       10000 //Colunas matriz
#define SUB_LINHAS    100   //linhas submatriz
#define SUB_COLUNAS   100   //Colunas submatriz
#define SEMENTE       1233  //Semente para gerar numeros aleat�rios fixos
#define QUANT_THREADS 2     //Define Quantidade das Threads criadas


int quantidade_sub_matrizes = 0, contador = 0, totalPrimos = 0;
int blocoAtual = 0;
pthread_mutex_t mutexBloco;
int** matriz = NULL;

int qtd_submatrizes(int i, int j, int sub_i, int sub_j) {
	int submatrizesLinhas = (i + sub_i - 1) / sub_i;//Mesmo Calculo que aparece na Fun��o subMatrizes
    int submatrizesColunas = (j + sub_j - 1) / sub_j;//Mesmo Calculo que aparece na Fun��o subMatrizes
	return submatrizesLinhas * submatrizesColunas;//Retorna a quantidade total de sub-matrizes baseado na quantidade de sublinhas e subcolunas que cabem na matriz
}

// ALOCAR ESPA�O PARA MATRIZ
int** alocarMatriz() {
    int i, j;

    // ALOCAR ESPA�O PARA LINHAS
    matriz = calloc(LINHAS, sizeof(int*));
    if (matriz == NULL) {
        printf("Erro ao alocar mem�ria.\n");
        return (NULL);
    }

    // ALOCAR ESPA�O PARA COLUNAS
    for (i = 0; i < LINHAS; i++) {
        matriz[i] = (int*)malloc(COLUNAS * sizeof(int));
        if (matriz[i] == NULL) {
            printf("Erro ao alocar mem�ria.\n");
            return (NULL);
        }
    }

    return matriz;
}

// PREENCHER MATRIZ COM N�MEROS ALEAT�RIOS
int** preencheMatriz(){


	matriz = alocarMatriz();//Alocar espa�o para matriz usando fun��o alocarMatriz

	//srand(SEMENTE);//Gerador de n�meros aleat�rios fixos, para usar descomentar essa linha e comentar a linha de baixo
	srand(time(NULL));//Gerador de n�meros aleat�rios, para usar descomentar essa linha, comentar a linha de cima

	for (int i = 0; i < LINHAS; i++) {//For para preencher a matriz
        for (int j = 0; j < COLUNAS; j++) {
            matriz[i][j] = rand() % 32000;
        }
    }

	return  matriz;//Retorna a matriz preenchida
}

// LIBERAR MEM�RIA ALOCADA
void liberarMatriz() {
    for (int i = 0; i < LINHAS; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

// RETORNAR SE UM N�MERO � OU N�O PRIMO
int ehPrimo(int numero) {
    if (numero <= 1) { return 0; }
    int raiz = (int)sqrt(numero);

    for (int i = 2; i <= raiz; i++) {
        if (numero % i == 0) { return 0; }
    }

    return 1;
}

//BUSCA SERIAL
void buscaSerial(int** matriz) {


	clock_t inicio, fim;//Vari�veis que v�o ser utilizadas para calcular o tempo de execu��o
	double tempo_gasto;//Vari�vel para guardar tempo total de execu��o
    inicio = clock();//Inicia contagem de tempo

	for (int i = 0; i < LINHAS; i++) {//For para percorrer matriz normal, e dizer qual n�mero � primo
        for (int j = 0; j < COLUNAS; j++) {
            totalPrimos += ehPrimo(matriz[i][j]);
        }
    }
	fim = clock();//Termina contagem de tempo
	tempo_gasto = ((double)(fim - inicio)) / CLOCKS_PER_SEC;//Divide tempo em quantidade de ticks por segundo, para mostrar tempo total em segundos

	printf("Tempo total busca serial: %.3f segundos\n", tempo_gasto);//Imprime tempo total de execu��o
	printf("Quantidade de primos: %d\n", totalPrimos);//Imprime quantidade total de primos na matriz

}

//L�GICA PARA CADA THREAD CONTAR PRIMOS EM SUBMATRIZES
int subMatrizes(int blocoLocal) {
	int linhaInicial, linhaFinal;//Define ponto inicial e final das linhas de uma submatriz
    int colunaInicial, colunaFinal;//Define ponto inicial e final das colunas de uma submatriz

	int submatrizesLinhas = (LINHAS + SUB_LINHAS - 1) / SUB_LINHAS;//Quantidade de submatrizes linhas que v�o caber na matriz -> Estava sendo utilizado para impress�o de debug
    int submatrizesColunas = (COLUNAS + SUB_COLUNAS - 1) / SUB_COLUNAS;//Quantidade de submatrizes colunas que v�o caber na matriz

	int contadorSubmatrizes = 0;//Contador para numerar em qual submatriz estamos -> Estava sendo utilizado para impress�o de debug

	int numPrimosLocal = 0;//Vari�vel para contar quantos primos tem na submatriz atual

    int i = blocoLocal / submatrizesColunas;//Devolve qual linha da submatriz estamos
    int j = blocoLocal % submatrizesColunas;//Devolve qual coluna da submatriz estamos

	linhaInicial = i * SUB_LINHAS;//ex. 0*250 = 0, logo a linha inicial da primeira submatriz � 0
	linhaFinal = linhaInicial + SUB_LINHAS - 1;//ex. 0+250-1 = 249, logo a linha final da primeira submatriz � 249
	if (linhaFinal >= LINHAS) linhaFinal = LINHAS - 1;//Caso a linha final ultrapasse o limite da matriz, ajusta para o limite m�ximo

	colunaInicial = j * SUB_COLUNAS;//ex. 0*100 = 0, logo a coluna inicial da primeira submatriz � 0
	colunaFinal = colunaInicial + SUB_COLUNAS - 1;//ex. 0+100-1 = 99, logo a coluna final da primeira submatriz � 99
	if (colunaFinal >= COLUNAS) colunaFinal = COLUNAS - 1;//Caso a coluna final ultrapasse o limite da matriz, ajusta para o limite m�ximo


	//printf("\nSub-matriz %d: Linhas [%d-%d], Colunas [%d-%d]\n", contadorSubmatrizes, linhaInicial, linhaFinal, colunaInicial, colunaFinal);//Imprime os limites da submatriz atual -> Estava sendo utilizado para impress�o de debug
            
	//For para passar por cada elemento dentro da submatriz atual
    for (int bi = linhaInicial; bi <= linhaFinal; bi++){
        for(int ji = colunaInicial; ji <= colunaFinal; ji++){
			numPrimosLocal += ehPrimo(matriz[bi][ji]);//Verifica se o n�mero � primo e incrementa o total de primos
		}
    }

    return numPrimosLocal;//Retorna o total de primos da submatriz atual

}

//FUN��O DAS THREADS -> O que cada thread vai executar
void* funcaoThreadBusca(void* arg) {
    
    int numPrimosSubMatriz = 0;//Variavel para contar quantos primos a thread encontrou
	int blocoLocal = 0;//Variavel para definir a submatriz que a thread vai analisar

    while(1){
		pthread_mutex_lock(&mutexBloco);//Mutex � travado para que outra thread n�o interfira na vari�vel contador

        if (blocoAtual >= qtd_submatrizes(LINHAS, COLUNAS, SUB_LINHAS, SUB_COLUNAS)){
			pthread_mutex_unlock(&mutexBloco);//Destrava mutex antes de sair do loop
            break;//Termina loop pois j� passou por todas submatrizes 
        }
        
        blocoLocal = blocoAtual++;//blocoLocal recebe o valor de blocoAtual e depois incementa blocoAtual para proxima thread
		pthread_mutex_unlock(&mutexBloco);//Destrava mutex para que outra thread possa acessar a vari�vel contador

		numPrimosSubMatriz += subMatrizes(blocoLocal);//Chama fun��o subMatrizes passando o bloco local que a thread vai processar(achar total de primos)

    }

	pthread_mutex_lock(&mutexBloco);//Trava mutex para atualizar vari�vel totalPrimos
	totalPrimos += numPrimosSubMatriz;//Atualiza total de primos com o valor local da thread
	pthread_mutex_unlock(&mutexBloco);//Destrava mutex ap�s atualizar vari�vel totalPrimos
    
    return NULL;
}

//BUSCA PARALELA -> Inicia as threads e gerencia o tempo de execu��o
void buscaParalela(int** matriz) {


    pthread_mutex_init(&mutexBloco, NULL);//Comando para iniciar mutex
    
    clock_t inicio, fim;//Vari�veis que v�o ser utilizadas para calcular o tempo de execu��o
    double tempo_gasto;//Vari�vel para guardar tempo total de execu��o
    inicio = clock();//Inicia contagem de tempo

    int arrayPos;
    //CRIAR THREADS
	pthread_t arrayThreads[QUANT_THREADS];//Cria array thread baseado na quantidade definida no #define
    
	for (arrayPos = 0; arrayPos < QUANT_THREADS; arrayPos++) {//Cria as threads
        pthread_create(&arrayThreads[arrayPos], NULL, funcaoThreadBusca, NULL);
    }
	for (arrayPos = 0; arrayPos < QUANT_THREADS; arrayPos++) {//Espera as threads terminarem
        pthread_join(arrayThreads[arrayPos], NULL);
    }

    fim = clock();//Termina contagem de tempo
    tempo_gasto = ((double)(fim - inicio)) / CLOCKS_PER_SEC;//Divide tempo em quantidade de ticks por segundo, para mostrar tempo total em segundos

    printf("\nTempo total busca paralela: %.3f segundos\n", tempo_gasto);//Imprime tempo total de execu��o
    printf("Quantidade de primos: %d\n", totalPrimos);//Imprime quantidade total de primos na matriz

	pthread_mutex_destroy(&mutexBloco);//Libera mem�ria alocada para mutex apos utilizada

}

// FUN��O PRINCIPAL
int main() {

	int** matriz = preencheMatriz();//Chama fun��o para preencher matriz

    printf("\nTamanho da matriz: %d linhas por %d colunas\n", LINHAS, COLUNAS);//Imprime tamanho total da matriz
	printf("\nQuantidade de submatrizes criadas: %d", qtd_submatrizes(LINHAS, COLUNAS, SUB_LINHAS, SUB_COLUNAS));//Total de submatrizes criadas
	printf("\nQuantidade threads criadas: %d\n", QUANT_THREADS);//Total de threads criadas
    printf("\n________________________________________________________________________\n");
    
    buscaSerial(matriz);
    totalPrimos = 0;//Zera total de primos para a pr�xima fun��o

    buscaParalela(matriz);
	totalPrimos = 0;//Zera total de primos para a pr�xima fun��o

    liberarMatriz(matriz, LINHAS);//Libera mem�ria alocada para matriz

    return 0;

}