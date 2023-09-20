#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int nThreads = 0;
int aux = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;


//função de barreira
void barreira(){
	if(aux < nThreads){
		pthread_cond_wait(&cond, &mutex);
	}

	else if(aux == nThreads){
		aux = 0;
		pthread_cond_broadcast(&cond);
		//esse puts(""); é só para ficar com saída mais legível
		//não é "de fato" parte da implementação da função barreira
		puts("");
	}
}

//função que as threads executarão
void* imprimeMensagens(void* n){
	long int nro = (long int) n;
	
	//primeiro print
	pthread_mutex_lock(&mutex);
	aux++;
	printf("Ola da thread %ld\n", nro);
	barreira();
	pthread_mutex_unlock(&mutex);

	//segundo print
	pthread_mutex_lock(&mutex);
	aux++;
	printf("Que dia bonito %ld\n", nro);
	barreira();
	pthread_mutex_unlock(&mutex);
	
	//terceiro print
	pthread_mutex_lock(&mutex);
	aux++;
	printf("Ate breve da thread %ld\n", nro);
	barreira();
	pthread_mutex_unlock(&mutex);
	
	pthread_exit(NULL);
}

int main(int argc, char** argv){
	int i = 0;

	//checa os argumentos vindos na linha de comando
	if(argc < 2){puts("Argumentos faltando!!"); return 1;}
	else{nThreads = atoi(argv[1]);}

	//inicializa vetor de ids de threads
	pthread_t* threadId = (pthread_t*) malloc(sizeof(pthread_t) * nThreads);
	if(threadId == NULL) {puts("ERRO: MALLOC"); return 2;}

	//inicializa o mutex e a variável de condição
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	//cria as threads
	for(i = 0; i < nThreads; i++){
		if(pthread_create(&threadId[i], NULL, imprimeMensagens, (void*) i)){
			puts("ERRO: CREATE");
			return 3;
		}
	}

	//espera todas as threads terminarem de executar
	for(i = 0; i < nThreads; i++){
		if(pthread_join(threadId[i], NULL)){
			puts("ERRO: JOIN");
			return 4;
		}
	}

	//libera espaço
	free(threadId);
	return 0;
}