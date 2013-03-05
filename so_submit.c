#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct entrada{
	char *nome;
	void *args;
	int tempo;
	int num_proc;
	}entrada;

int main (int argc, char* argv[]){
	
	FILE* fp;
	char arq[50], conteudo[50];
	entrada entrada;
	int max_Time;
	
	strcpy(arq, argv[1]);

	fp = fopen(arq, "r");
	
	/*acesso aos dados do arquivo*/
	while(fscanf(fp, "%s",  conteudo) != EOF){
		//printf("conteudo: %s\n",conteudo);	
		
		/*Para saber o que acessa*/
		if(strcmp(conteudo, "Program") == 0){
			fscanf(fp, "%s",  conteudo); /*pega o = e joga fora*/
			fscanf(fp, "%s",  entrada.nome);/*pego o nome*/
			fscanf(fp, "%[^\n]s",  (char *)entrada.args);/*pego o conteudo*/
		}
		if(strcmp(conteudo, "Max_Time") == 0){
			fscanf(fp, "%s",  conteudo); /*pega o = e joga fora*/
			fscanf(fp, "%d", max_time);/*pego o max_time*/
		}
		if(strcmp(conteudo, "Max_Time") == 0){
			fscanf(fp, "%s",  conteudo); /*pega o = e joga fora*/
			fscanf(fp, "%d",  num_proc);/*pego o max_time*/
		}
	}
	
	return 0;
}