#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>



typedef struct processo{
	char proc[50];
	int max_time;
	int num_proc;
	time_t start_time;
}PROCESSO_T;


void getValue(FILE **fp,char* str){

	fscanf(*fp,"%s",str);
	fscanf(*fp,"%s",str);getc(*fp);
	fscanf(*fp,"%[^\n]s",str);

}


int std2sec(char *std_time){

	char hr[3],min[3],sec[3];
	int time;



	strncpy(hr,std_time,2);
	strncpy(min,std_time+3,2);
	strncpy(sec,std_time+6,2);

	hr[2] = 0;
	min[2] = 0;
	sec[2] = 0;

	time = atoi(hr)*3600 + atoi(min)*60 + atoi(sec);


	return time;

}


void printProcesso(PROCESSO_T processo){

	int i;

	printf("Program = %s",processo.proc);
	printf("\nMax_Time = %d\n",processo.max_time);
	printf("N_proc = %d\n",processo.num_proc);

}


int main (int argc, char* argv[]){
	
	FILE* fp;
	char arqName[50], temp[50];
	int i,n_args;
	PROCESSO_T processo;

	
	strcpy(arqName, argv[1]);

	fp = fopen(arqName, "r");
	if(fp == NULL){
		printf("Arquivo nao encontrado\n");
		exit(-1);

	}

	while(!feof(fp)){

		getValue(&fp,processo.proc);/*pega o nome do programa*/

		getValue(&fp,temp);
		processo.max_time = std2sec(temp);/*Pega o valor do max_time e converte para segundos*/

		getValue(&fp,temp);
		processo.num_proc = atoi(temp);/*Pega o numero de processos e transforma em inteiro*/


		processo.start_time = time(NULL);/*Pega o tempo de inicio da execução*/
		
		
	}




	fclose(fp);
	return 0;
}
