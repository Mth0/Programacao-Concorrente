/*
----------IMPORTANTE!!!!----------

Os testes de tempo, aceleracao e eficiencia estao no arquivo "lab2_calculaMedias.c". Achei melhor
criar um arquivo para isso por questoes de organizacao.

----------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>


//variaveis globais
long long int NElem = 0;
int Nthreads = 0;
long long int iGlobal = 0;
long long int totalGlobal = 0;
pthread_mutex_t bastao;
int* sequencia;

//funcao para gerar um vetor sequencial de tamanho NElem
int* geraVetorSequencial(){
	for(int i = 0; i < NElem; i++){
		sequencia[i] = i+1;
	}
	return sequencia;
}


//funcao para checar se um n eh primo
int ehPrimo(long long int n) {
	int i;
	if (n<=1) return 0;
	if (n==2) return 1;
	if (n%2==0) return 0;
	for (i=3; i<sqrt(n)+1; i+=2)
		if(n%i==0) return 0;
	return 1;
}

//funcao que as threads executaram, cada thread tera seu calculo de primos encontrados
void* contaPrimos(void* arg){
	long long int iLocal = 0;
	long long int totalLocal = 0;
	
	//exclusao mutua
	pthread_mutex_lock(&bastao);
	iLocal = iGlobal;
	iGlobal++;
	pthread_mutex_unlock(&bastao);

	while(iLocal < NElem){
		if(ehPrimo(sequencia[iLocal])) totalLocal++;
		//exclusao mutua
		pthread_mutex_lock(&bastao);
		iLocal = iGlobal;
		iGlobal++;
		pthread_mutex_unlock(&bastao);
	}

	//exclusao mutua
	pthread_mutex_lock(&bastao);
	totalGlobal += totalLocal;
	pthread_mutex_unlock(&bastao);

	pthread_exit(NULL);
}

int main(int argc, char* argv[]){
	double inicio, fim, delta;
	GET_TIME(inicio);
	if(argc < 3)
		printf("Não ta esquecendo de nada nao? Digite <numero de elementos> <numero de threads>\n");

	//inicializa valores vindos do argv
	NElem = atoll(argv[1]);
	Nthreads = atoi(argv[2]);

	//cria vetor de identificadores para threads
	pthread_t *tid;
	tid = (pthread_t*) malloc(sizeof(pthread_t)*Nthreads);

	//inicializa o "mediador" da exclusao mutua
	pthread_mutex_init(&bastao, NULL);

	//inicializa o vetor sequencial que sera usado
	sequencia = (int*) malloc(sizeof(int) * NElem);
	if(sequencia == NULL){printf("ERRO: malloc"); return 1;}
	sequencia = geraVetorSequencial();
	GET_TIME(fim);

	delta = fim - inicio;
	printf("Tempo inicializacao: %lf\n", delta);

	GET_TIME(inicio);
	//cria as threads e as depacha para "suas" funcoes
	for(int i = 0; i < Nthreads; i++){
		if(pthread_create(&tid[i], NULL, contaPrimos, NULL)){
			printf("ERRO: pthread_create"); return 2;
		}
	}

	//aguarda todos acabarem
	for(int i = 0; i < Nthreads; i++){
		pthread_join(tid[i], NULL);
	}
	GET_TIME(fim);
	delta = fim - inicio;
	printf("Tempo de execucao com %d threads e %lld elementos: %lf\n", Nthreads, NElem, delta);

	//printa o resultado
	printf("No intervalo [1, %lld] ha %lld primos!\n\n\n\n", NElem, totalGlobal);

	//libera espaco
	free(tid);
	free(sequencia);
}