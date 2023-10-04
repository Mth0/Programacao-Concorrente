/*
----------IMPORTANTE!!!!----------

Infelizmente minha implementação não lê a última linha do arquivo :(. Desculpe.

Tentei corrigir, mas não consegui.... Deixei de um modo que até a penúltima
linha seja lida e printada corretamente.

----------------------------------
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

int N;
int threadsAtivas;
int tamanhoBuffer;
char** buffer;

sem_t slotVazio;
sem_t slotCheio;
sem_t mutexConsumo;

int fimDeArquivo = 0;

FILE* f;


//le linha por linha do arquivo
char* leLinha(){
	//declara o tam da linha e um char que será lido
	static int linhaTam = 0;
	char c = 'a';
	
	//cada char que não seja quebra de linha ou fim de string é contabilizado
	while(c != '\n' && c != '\0'){
		c = fgetc(f);
		if(feof(f)){
			sem_wait(&mutexConsumo);
			fimDeArquivo = 1;
			sem_post(&mutexConsumo);
			break;
		}
		linhaTam++;
	}

	//aqui aloca linha, retrocede o ponteiro em tamanho da linhaTam + 2
	//e depois lê do inicio da linha até linhaTam + 2 posições adiante
	//o + 2 serve para o \n do fim da linha e
	//voltar uma posição para trás do ponteiro, pois ele está apontando
	//para o primeiro elemento da próxima linha
	c = fgetc(f);
	linhaTam+=2;
	
	char* linha = (char*) malloc(sizeof(char) * (linhaTam + 1));

	fseek(f, -linhaTam, SEEK_CUR);
	fgets(linha, linhaTam, f);
	fseek(f, 0, SEEK_CUR);
	linhaTam = 0;
	
	return linha;
}

//definição de funções auxiliares de semáforos
void entraProducao(){
	sem_wait(&slotVazio);
	sem_wait(&mutexConsumo);
}

void saiProducao(){
	sem_post(&mutexConsumo);
	sem_post(&slotCheio);
}

void entraConsome(){
	sem_wait(&slotCheio);
	sem_wait(&mutexConsumo);
}

void saiConsome(){
	sem_post(&mutexConsumo);
	sem_post(&slotVazio);
}

//função dos consumidores
void consome(){
	static int out = 0;
	char* valor;
	entraConsome();
	valor = buffer[out];
	out = (out + 1) % tamanhoBuffer;
	printf("%s", valor);
	saiConsome();
}

//função das threads. Basicamente consomem em loop
//até que seja sinalizado que o arquivo terminou
void* consumidora(){
	while(fimDeArquivo == 0){
		consome();
	}

	//decrementa o número de threads ativas
	sem_wait(&mutexConsumo);
	threadsAtivas--;
	printf("\n\nTerminei!!!!");
	sem_post(&mutexConsumo);
	//setbuf(stdout, NULL);
	pthread_exit(NULL);
}

int main(int argc, char** argv){
	tamanhoBuffer = 5;
	//alocação para o nome do arquivo
	char* arquivoNome = (char*) malloc(sizeof(char) * 100);

	if(arquivoNome == NULL){
		puts("Erro de alocacao para o nome do arquivo :(.");
		return 1;
	}

	//recebe argumentos da linha de comando
	if(argc < 3){
		puts("Esqueceu de algum argumento! Formato: <número de threads> <nome do arquivo> <tamanho do buffer (opcional)>");
		return 2;
	}
	else if(argc == 3){
		N = atoi(argv[1]);
		arquivoNome = argv[2];
	}
	else{
		N = atoi(argv[1]);
		arquivoNome = argv[2];
		tamanhoBuffer = atoi(argv[3]);
	}

	//inicializa de coisas importantes
	pthread_t* tid = (pthread_t*) malloc(sizeof(pthread_t) * N);
	buffer = (char**) malloc(sizeof(char*) * tamanhoBuffer);

	if(tid == NULL){
		puts("Erro na alocação de espaço para tid!");
		return 3;
	}
	else if(buffer == NULL){
		puts("Erro na alocação de espaço para o buffer!");
		return 4;
	}

	//abre o arquivo
	f = fopen(arquivoNome, "r");

	if(f == NULL){
		printf("Erro no carregamento do arquivo \"%s\"!\n", arquivoNome);
		return 5;
	}

	//inicializa semáforos
	sem_init(&slotCheio, 0, 0);
	sem_init(&slotVazio, 0, tamanhoBuffer);
	sem_init(&mutexConsumo, 0, 1);


	//cria as threads
	threadsAtivas = N;
	for(int i = 0; i < N; i++){
		if(pthread_create(&tid[i], NULL, consumidora, NULL)){
			printf("Erro na criação da thread %d", i + 1);
			return 6;
		}
	}

	int in = 0;
	char* linha;

	//le linha por linha do arquivo e só para se o fim
	//do arquivo for alcançado ou todas as threads pararem
	while(!feof(f) && threadsAtivas > 0){
		linha = leLinha();
		//puts("a");
		entraProducao();
		buffer[in] = linha;
		in = (in + 1) % tamanhoBuffer;
		//printBuffer(buffer, N);
		//printf("%d", in);
		saiProducao();
	}
	fimDeArquivo = 1;

	//aguarda todas as threads pararem
	//teoricamente não seria necessário este for
	for(int i = 0; i < N; i++){
		if(pthread_join(tid[i], NULL)){
			printf("Erro de join na thread %d", i + 1);
			return 7;
		}
	}


	fclose(f);

	free(tid);
	return 0;
}