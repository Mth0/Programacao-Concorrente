/*
----------IMPORTANTE!!!!----------

Esses testes foram feitos em um compilador online de C.

Falando um pouco sobre o resultado, achei interessante o fato de 2 threads ser consideravelmente melhor
do que 4. Além disso, o tempo de inicializacao nao interferiu muito no calculo geral das medias, o que faz
sentido por ser uma parte sequencial....

----------------------------------
*/

#include <stdio.h>

int main(){
	//inicializando vetores com os tempos coletados
	float uma_t_ini[5] = {0.003919, 0.005805, 0.003806, 0.005829, 0.008323};
	float uma_t_exe[5] = {0.135982, 0.492964, 0.142050, 0.621446, 0.580419};

	float duas_t_ini[5] = {0.003628, 0.004141, 0.005628, 0.003685, 0.004944};
	float duas_t_exe[5] = {0.091359, 0.111614, 0.560592, 0.151228, 0.246594};

	float quatro_t_ini[5] = {0.004028, 0.004180, 0.003996, 0.003820, 0.006156};
	float quatro_t_exe[5] = {0.269142, 0.243854, 0.263083, 0.215264, 0.686930};

	//inicializando medias
	float uma_t_iniMedia = 0;
	float uma_t_exeMedia = 0;

	float duas_t_iniMedia = 0;
	float duas_t_exeMedia = 0;

	float quatro_t_iniMedia = 0;
	float quatro_t_exeMedia = 0;

	//calculando medias
	for(int i = 0; i < 5; i++){
		uma_t_iniMedia += uma_t_ini[i];
		uma_t_exeMedia += uma_t_exe[i];

		duas_t_iniMedia += duas_t_ini[i];
		duas_t_exeMedia += duas_t_exe[i];

		quatro_t_iniMedia += quatro_t_ini[i];
		quatro_t_exeMedia += quatro_t_exe[i];
	}

	uma_t_iniMedia /= 5;
	duas_t_iniMedia /= 5;
	quatro_t_iniMedia /= 5;

	uma_t_exeMedia /= 5;
	duas_t_exeMedia /= 5;
	quatro_t_exeMedia /= 5;
	
	printf("Tempo medio de inicializacao com 1 thread (\"sequencial\"): %f\n", uma_t_iniMedia);
	printf("Tempo medio de inicializacao com 2 threads: %f\n", duas_t_iniMedia);
	printf("Tempo medio de inicializacao com 4 threads: %f\n\n", quatro_t_iniMedia);

	printf("Tempo medio de execucao com 1 thread (\"sequencial\"): %f\n", uma_t_exeMedia);
	printf("Tempo medio de execucao com 2 threads: %f\n", duas_t_exeMedia);
	printf("Tempo medio de execucao com 4 threads: %f\n\n", quatro_t_exeMedia);

	//inicializando aceleracao
	float duas_t_aceleracao = 0;
	float quatro_t_aceleracao = 0;

	printf("-----MEIDADAS SO CONSIDERANDO O TEMPO DE INICIALIZACAO-----\n");
	printf("(ACHO QUE NAO FAZ MUITO SENTIDO, MAS ACHEI INTERESSANTE TER TAMBEM)\n\n");
	duas_t_aceleracao = uma_t_iniMedia / duas_t_iniMedia;
	quatro_t_aceleracao = uma_t_iniMedia / quatro_t_iniMedia;
	printf("Aceleracao com 2 threads: %f\n", duas_t_aceleracao);
	printf("Aceleracao com 4 threads: %f\n\n", quatro_t_aceleracao);

	printf("Eficiencia com 2 threads: %f\n", duas_t_aceleracao / 2);
	printf("Eficiencia com 4 threads: %f\n\n", quatro_t_aceleracao / 4);

	printf("-----MEIDADAS SO CONSIDERANDO O TEMPO DE EXECUCAO-----\n\n");
	duas_t_aceleracao = uma_t_exeMedia / duas_t_exeMedia;
	quatro_t_aceleracao = uma_t_exeMedia / quatro_t_exeMedia;
	printf("Aceleracao com 2 threads: %f\n", duas_t_aceleracao);
	printf("Aceleracao com 4 threads: %f\n\n", quatro_t_aceleracao);

	printf("Eficiencia com 2 threads: %f\n", duas_t_aceleracao / 2);
	printf("Eficiencia com 4 threads: %f\n\n", quatro_t_aceleracao / 4);

	printf("-----MEDIDAS CONSIDERANDO AMBOS OS TEMPOS-----\n\n");
	duas_t_aceleracao = (uma_t_iniMedia + uma_t_exeMedia) / (duas_t_iniMedia + duas_t_exeMedia);
	quatro_t_aceleracao = (uma_t_iniMedia + uma_t_exeMedia) / (quatro_t_iniMedia + quatro_t_exeMedia);
	printf("Aceleracao com 2 threads: %f\n", duas_t_aceleracao);
	printf("Aceleracao com 4 threads: %f\n\n", quatro_t_aceleracao);

	printf("Eficiencia com 2 threads: %f\n", duas_t_aceleracao / 2);
	printf("Eficiencia com 4 threads: %f\n", quatro_t_aceleracao / 4);
}
