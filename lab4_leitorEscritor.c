#include <stdio.h>
#include <pthread.h>
#include "lab4_leitorEscritor.h"

pthread_mutex_t mutex;
pthread_cond_t condLeitura;
pthread_cond_t condEscrita;

int leitor = 0;
int escritor = 0;


//funcoes de leitura
void entraLeitura(){
	pthread_mutex_lock(&mutex);
	while(escritor > 0){
		pthread_cond_wait(&condLeitura, &mutex);
	}
	leitor++;
	pthread_mutex_unlock(&mutex);
}

void saiLeitura(){
	pthread_mutex_lock(&mutex);
	leitor--;
	if(leitor == 0){
		pthread_cond_signal(&condEscrita);
	}
	pthread_mutex_unlock(&mutex);
}


//funcoes de escrita
void entraEscrita(){
	pthread_mutex_lock(&mutex);
	while(leitor > 0 || escritor > 0){
		pthread_cond_wait(&condEscrita, &mutex);
	}
	escritor++;
	pthread_mutex_unlock(&mutex);
}

void saiEscrita(){
	pthread_mutex_lock(&mutex);
	escritor--;
	if(escritor == 0){
		pthread_cond_signal(&condEscrita);
		pthread_cond_broadcast(&condLeitura);
	}
	pthread_mutex_unlock(&mutex);
}
