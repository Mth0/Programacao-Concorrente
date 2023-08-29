/*
----------IMPORTANTE!!!!----------

O tamanho do vetor é definido pela macro TAMANHOVETOR, porém crio um vetor com o dobro
de tamanho desse valor com o intuito de ter uma "cópia" dele inalterada para comparar
os valores ao fim do arquivo na função de verificação.

----------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define TAMANHOVETOR 10000
#define NUMEROTHREADS 8

typedef struct{
	int inicio;
	long int* vet;
}tArgs;


//inicializa um vetor generico utilizando números aleatórios
//usando a função rand()
long int* incializaVetor(){
	long int* vet = (long int*) malloc(2 * TAMANHOVETOR * sizeof(long int*));

	srand(time(NULL));
	long int valor = 0;

	for(int i = 0; i < TAMANHOVETOR; i++){
		valor = rand();
		vet[i] = valor;
		vet[i + TAMANHOVETOR] = valor;
	}

	return vet;
}

//eleva elementos de um vetor ao quadrado
void *elevaQuadrado(void* arg){
	tArgs* argumentos = (tArgs*) arg;

	printf("Thread %d criada e cobrira os indices de %d em %d a partir do indice %d\n\n",
		argumentos->inicio + 1, NUMEROTHREADS, NUMEROTHREADS, argumentos->inicio);

	int tam = argumentos->inicio;

	while(tam < TAMANHOVETOR){
		argumentos->vet[tam] *= argumentos->vet[tam];
		tam += NUMEROTHREADS;
	}

	printf("Thread %d terminou as operacoes e sera encerrada :(\n\n",
		argumentos->inicio + 1);

	free(argumentos);
	pthread_exit(NULL);
}

int verifica(long int* vet){

	//verifica para cada elemento do vetor se o cálculo está correto.
	//A verificação é feita checando se ni^2 == n, onde ni == numero inalterado
	for(int i = 0; i < TAMANHOVETOR; i++){
		if(vet[i] == 0){
			printf("%ld, %ld, %d", vet[i], vet[i + TAMANHOVETOR], i);
		}
		//checa o valor
		if(vet[i + TAMANHOVETOR] * vet[i + TAMANHOVETOR] != vet[i]){
			printf("%ld / %ld == %ld\n",
				vet[i], vet[i + TAMANHOVETOR], vet[i] / vet[i + TAMANHOVETOR]);
			return 0;
		}
	}

	return 1;
}

int main(void){
	long int* vet = incializaVetor();

	/*for(int i = 0; i < 2*TAMANHOVETOR; i++){
		printf("%f\n", vet[i]);
	}*/

	//cria um vetor de identificadores das threads
	pthread_t tidSistema[NUMEROTHREADS];

	for(int i = 0; i < NUMEROTHREADS; i++) {

		//inicializa os argumentos de uma thread
		tArgs *argumentos = malloc(sizeof(tArgs));
		if (argumentos == NULL) {
      		printf("--ERRO: malloc()\n"); exit(-1);
    	}
		argumentos->inicio = i;
		argumentos->vet = vet;


		//cria a thread e passa a função de elevar ao quadrado para ela
		if(pthread_create(&tidSistema[i], NULL, elevaQuadrado, (void*) argumentos)) {
      		printf("--ERRO: pthread_create()\n"); exit(-1);
    	}
	}

	//espera as threads terminarem
	for(int i = 0; i < NUMEROTHREADS; i++){
		if(pthread_join(tidSistema[i], NULL)){
			printf("--ERRO: pthread_join()\n"); exit(-1);
		}
	}

	/*printf("\n");
	for(int i = 0; i < 2*TAMANHOVETOR; i++){
		printf("%f\n", vet[i]);
	}*/

	//verifica o resultado
	if(verifica(vet)) printf("Resultado correto!");
	else printf("Resultado incorreto!");

	//libera espaço
	free(vet);
	pthread_exit(NULL);
}