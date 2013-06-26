#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>



typedef struct entrada{
	char nome[50];
	char **args;
	int max_time;
	int num_proc;
}ENTRADA_T;


void getValue(FILE **fp,char* str){

	fscanf(*fp,"%s",str);
	fscanf(*fp,"%s",str);
	fscanf(*fp,"%s",str);

}

int getArgs(FILE **fp,char ***args){

	int i=0,n_args=0;
	long pos;
	char c;


	pos = ftell(*fp);

	while((c = getc(*fp)) != '\n'){
		if(c == ' '){
			n_args++;
		}

	}


	fseek(*fp,pos,SEEK_SET);

	*args = NULL;
	*args = (char**)calloc(n_args,sizeof(char*));

	(*args)[0] = NULL;
	for(i=0;i<n_args;i++){
		(*args)[i+1] = NULL;
		(*args)[i] = (char*)calloc(50,sizeof(char));
		fscanf(*fp,"%s",(*args)[i]);
		
	}


	return n_args;
	

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


void printEntrada(ENTRADA_T entrada){

	int i;

	printf("Program = %s",entrada.nome);
	for(i=0;entrada.args[i];i++){
		printf(" %s",entrada.args[i]);
	}
	printf("\nMax_Time = %d\n",entrada.max_time);
	printf("N_proc = %d\n",entrada.num_proc);

}


int main (int argc, char* argv[]){
	
	FILE* fp;
	char arqName[50], temp[50];
	int i,n_args;
	ENTRADA_T entrada;
	
	strcpy(arqName, argv[1]);

	fp = fopen(arqName, "r");
	if(fp == NULL){
		printf("Arquivo nao encontrado\n");
		exit(-1);

	}

	while(!feof(fp)){

		getValue(&fp,entrada.nome);/*pega o nome do programa*/

		n_args = getArgs(&fp,&entrada.args);/*Pega os argumentos do programa e retorna o numero de argumentos total*/

		getValue(&fp,temp);
		entrada.max_time = std2sec(temp);/*Pega o valor do max_time e converte para segundos*/

		getValue(&fp,temp);
		entrada.num_proc = atoi(temp);/*Pega o numero de processos e transforma em inteiro*/

		printEntrada(entrada);/*Printa na tela o conteudo da entrada*/

		

	}




	fclose(fp);
	return 0;
}
