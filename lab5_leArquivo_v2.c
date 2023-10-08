/*
----------IMPORTANTE!!!!----------

Essa versão é a v2 (ou até maior) e foi postada após o prazo.

----------------------------------
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

int N;
int tamanhoBuffer;
int tamanhoLinhas;
char** buffer;
int slotCheios;

sem_t slotVazio;
sem_t slotCheio;
sem_t mutex;
sem_t mutex_impressao;

int fimDeArquivo = 0;

FILE* f;

//função de inicialização do buffer
char** inicializaBuffer(int tamBuf, int tamLin){ //recebe o tamanho do buffer e o tamanho das linhas
	//Silvana: nao é necessario/recomendavel usar variaveis globais aqui (use argumentos na funcao)
	//Matheus: feito! Adaptei para evitar o uso de variáveis globais

	//aloca o buffer
	char** buf = (char**) malloc(sizeof(char*) * tamBuf);
	if(buf == NULL){
		puts("Erro de alocacao do buffer **");
		exit(2);
	}
	//aloca cada posição do buffer
	for(int i = 0; i < tamBuf; i++){
		buf[i] = (char*) malloc(sizeof(char) * tamLin);
		if(buf[i] == NULL){
			puts("Erro de alocao do buffer *");
			exit(3);
		}
	}

	return buf;
}

//função para liberar o espaço do buffer
void freeBuffer(char** buf, int tamBuf){
	//Silvana: nao é necessario/recomendavel usar variaveis globais aqui (use argumentos na funcao)
	//Matheus: feito! Adaptei para evitar o uso de variáveis globais

	//libera o espaço de cada elemento do buffer
	for(int i = 0; i < tamBuf; i++){
		free(buf[i]);
	}

	//libera o espaço do buffer
	free(buf);
}

//le linha por linha do arquivo. Recebe um ponteiro de char para
//armazenar o conteudo da linha, o tamanho da linha a ser lida e o arquivo
//de onde será lida
char* leLinha(char* linha, int tamLin, FILE* arquivo){
	//static int lin = 0; //Silvana: para que esta sendo usada?
	//lin++; //Matheus: se não me engano havia a utilizado para testes somente. Esqueci de retirar, perdão
	
	//Silvana: nao é necessario/recomendavel usar variaveis globais aqui (use argumentos na funcao)
	//Matheus: feito! Adaptei para evitar o uso de variáveis globais
	if(fgets(linha, tamLin, arquivo) == NULL){
		return NULL;
	}
	else{
		return linha;
	}
}

//insere a uma linha no buffer
void insere(char* linha){
	static int in = 0;

	sem_wait(&slotVazio);
	sem_wait(&mutex);

	strcpy(buffer[in], linha);

	in = (in + 1) % tamanhoBuffer;
	slotCheios++;

	sem_post(&mutex);
	sem_post(&slotCheio);
}

//retira uma linha do buffer
void retira(char* linhaOut){
	static int out = 0;

	sem_wait(&slotCheio);
	sem_wait(&mutex);

	strcpy(linhaOut, buffer[out]);

	out = (out + 1) % tamanhoBuffer;
	slotCheios--;

	sem_post(&mutex);
	sem_post(&slotVazio);
}

//imprime uma mensagem
void imprime(char* mensagem){
	printf("%s", mensagem);
}

//função das threads. Basicamente consomem em loop
void* consumidora(){
	char* linhaRetirada = (char*) malloc(sizeof(char) * tamanhoLinhas);
	if(linhaRetirada == NULL){
		puts("Erro na alocacao de linhaRetirada");
		exit(4);
	}
	//while(fimDeArquivo == 0){ //Silvana: pode terminar antes de consumir todas as linhas do buffer!!!
	//O ideal seria essa checagem ser feita dentro do while? Tentei fazer isso, parece que deu certo, obrigado!
	//criei uma variável "sloCheios" que diz quantos slotsCheios há no momento
	while(1){
		sem_wait(&mutex_impressao); //Silvana: exclusao mutua para impressao

		if(fimDeArquivo == 1 && slotCheios == 0){
			break;
		}

		retira(linhaRetirada);
		imprime(linhaRetirada);
		

		sem_post(&mutex_impressao);
	}
	sem_post(&mutex_impressao);

	free(linhaRetirada);
	pthread_exit(NULL);
}

int main(int argc, char** argv){
	tamanhoBuffer = 5;
	tamanhoLinhas = 200;

	//alocação para o nome do arquivo
	char* arquivoNome = (char*) malloc(sizeof(char) * 100);
	if(arquivoNome == NULL){
		puts("Erro de alocacao para o nome do arquivo :(.");
		return 1;
	}

	//recebe argumentos da linha de comando
	if(argc < 3){
		puts("Esqueceu de algum argumento! Formato: <numero de threads> <nome do arquivo> <tamanho do buffer (opcional)> <tamanho das linhas do arquivo (opcional)>");
		return 2;
	}
	else if(argc == 3){
		N = atoi(argv[1]);
		arquivoNome = argv[2];
	}
	else if(argc == 4){
		N = atoi(argv[1]);
		arquivoNome = argv[2];
		tamanhoBuffer = atoi(argv[3]);
	}
	else{
		N = atoi(argv[1]);
		arquivoNome = argv[2];
		tamanhoBuffer = atoi(argv[3]);
		tamanhoLinhas = atoi(argv[4]);
	}

	//inicializa de coisas importantes
	pthread_t* tid = (pthread_t*) malloc(sizeof(pthread_t) * N);

	if(tid == NULL){
		puts("Erro na alocacao de espaco para tid!");
		return 3;
	}

	buffer = inicializaBuffer(tamanhoBuffer, tamanhoLinhas);
	slotCheios = 0;

	//abre o arquivo
	f = fopen(arquivoNome, "r");
	if(f == NULL){
		printf("Erro no carregamento do arquivo \"%s\"!\n", arquivoNome);
		return 4;
	}

	//inicializa semáforos
	sem_init(&slotCheio, 0, 0);
	sem_init(&slotVazio, 0, tamanhoBuffer);
	sem_init(&mutex, 0, 1);
	sem_init(&mutex_impressao, 0, 1);


	//cria as threads
	for(int i = 0; i < N; i++){
		if(pthread_create(&tid[i], NULL, consumidora, NULL)){
			printf("Erro na criacao da thread %d", i + 1);
			return 5;
		}
	}
	//Silvana: verificar se alocou com sucesso
	//feito! Havia, de fato, não verificado, perdão

	//cria a string linha que o loop abaixo usará
	char* linha = (char*) malloc(sizeof(char) * tamanhoLinhas);

	if(linha == NULL){
		puts("Erro na alocao de espaco para linha");
		return 6;
	}

	//le linha por linha do arquivo e só para se o fim
	//do arquivo for visto ou se a linha lida for nula.

    while(!feof(f)){ //Silvana: só vai acusar final de arquivo depois de uma tentativa de leitura que detecta o final do arquivo
		linha = leLinha(linha, tamanhoLinhas, f);
		if (linha!=NULL) insere(linha); //Silvana: alterei aqui: só insere se nao retornar NULL
	}

	//setta valor 1 para indicar que o arquivo terminou
	sem_wait(&mutex);
	fimDeArquivo = 1;
	sem_post(&mutex);

	//aguarda todas as threads pararem
	for(int i = 0; i < N; i++){
		if(pthread_join(tid[i], NULL)){
			printf("Erro de join na thread %d", i + 1);
			return 7;
		}
	}

	//fecha o arquivo e libera espaco
	fclose(f);
	free(linha);
	free(tid);
	freeBuffer(buffer, tamanhoBuffer);
	return 0;
}