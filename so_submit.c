/**

Cain� Felipe Bento Razzolini - 09/0108094
Yuri Cossich Lavinas - 09/0015266

Compilador: gcc 4.6.3 
	x86_64-Linux-gnu
 
SO: Ubuntu 12.04 LTS 64 bits

Algoritmo de escalonamento: FIFO
	O escalonador ir� buscar um arquivo na tabela de arquivos com o tempo de entrada mais antigo, caracterizando o FIFO. S� ser�
executado um arquivo por vez, mesmo que o max_proc n�o esteja todo ocupado.

Mecanismos PIC: 
	Foram usados 2 tipos de mecanismos IPC, sem�foros e mem�ria compartilhada. E para cada tipo, foram usadas duas inst�ncias.
	
		1.As mem�rias compartilhadas foi utilizada para termos acesso a tabela e vari�veis de controle de acesso e escrita.
	Em idshm foi criada para conter a tabela de processos, enquanto id2shm foi criada para, qudndo so_shutdown for executado, podermos
	evitar que novos processos possam ser ou executados ou entrarem na tabela de processos.

		2.J� os sem�foros foram utilizados para garantir que tenhamos somente um processo com acesso a dados/vari�veis vitais.	
	idsem foi criado para garantir acesso �nico as mem�rias compartilhadas. id2sem, para a quantidade de processos que podem ser alocados.

As estruturas de dados:
	INFO_T, PROCESSO_T, INFO_T, PROG_T, REQ_PIDS_T, PROCESSO_T, INFO_T, PROG_T REQ_PIDS_T

*/

#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define RUNNING 1
#define PENDING 0
#define NUM_TAB 50
#define NAO_PODE_ESCREVER -1

int idsem;
struct sembuf operacao[1];

typedef struct processo{
	int nreq;
	char max_time[9];
	int num_proc;
	time_t start_time;
	int status;
	char proc[50];
	int pid;/*pid do dispatcher*/
}PROCESSO_T;

typedef struct info{
	int write_permission;
	int last_nreq;
}INFO_T;

void p_sem()
{
     operacao[0].sem_num = 0;
     operacao[0].sem_op = 1;
     operacao[0].sem_flg = 0;
     if ( semop(idsem, operacao, 1) < 0)
       printf("erro no p=%d\n", errno);
}
void v_sem()
{
     operacao[0].sem_num = 0;
     operacao[0].sem_op = -1;
     operacao[0].sem_flg = 0;
     if ( semop(idsem, operacao, 1) < 0)
       printf("erro no p=%d\n", errno);
}



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


void printProcesso(PROCESSO_T processo1){
	printf("Program = %s",processo1.proc);
	printf("\nMax_Time = %s\n",processo1.max_time);
	printf("N_proc = %d\n",processo1.num_proc);
}




int main (int argc, char* argv[]){

	FILE* fp;
	char arqName[50], temp[50];
	PROCESSO_T aux;
	int idshm, id2shm;
	INFO_T *p2shm;
	PROCESSO_T *pshm,*paux;
	int i = 0;

	if(argc == 1){
		printf("Nenhum arquivo de entrada informado\n");
		exit(-1);
	}

	strcpy(arqName, argv[1]);

	fp = fopen(arqName, "r");
	if(fp == NULL){
		printf("Arquivo nao encontrado\n");
		exit(-1);
	}

	/*da um shmget na mem compartilhada*/	
	if ((idshm = shmget(90108094, NUM_TAB*sizeof(struct processo), IPC_CREAT|0x1ff)) < 0){
	    printf("erro na criacao da memoria compartilhada(idshm)\n");
	    exit(1);
	}

	/*da um attach na mem compartilhada*/
	pshm = (struct processo *) shmat(idshm, (char *)0, 0);
	if (pshm == (struct processo *)-1) {
        printf("erro no attach\n");
        exit(1);
    }

	/*da um shmget na mem compartilhada de bloqueio*/	
	if ((id2shm = shmget(90108012, sizeof(INFO_T), IPC_CREAT|0x1ff)) < 0){
	    printf("erro na criacao da memoria compartilhada(id2shm)\n");
	    exit(1);
	}

	/*da um attach na mem compartilhada de bloqueio para ver se pode escrever ou nao*/
	p2shm = (INFO_T *) shmat(id2shm, (char *)0, 0);
	if (p2shm == (INFO_T *)-1){
        printf("erro no attach\n");
        exit(1);
    }

	/*da um semget em semaforo para cria-lo*/
	if ((idsem = semget(90015266, 1, IPC_CREAT|0x1ff)) < 0){
	     printf("erro na criacao do semaforo\n");
	     exit(1);
	}


	getValue(&fp,aux.proc);/*pega o nome do programa*/
	getValue(&fp,aux.max_time);/*Pega o tempo m�ximo de execucao*/
	getValue(&fp,temp);
	aux.num_proc = atoi(temp);/*Pega o numero de processos e transforma em inteiro*/
	aux.start_time = time(NULL);/*Pega o tempo de inicio da execucao*/
	aux.status = PENDING;


	/*printProcesso(aux);*/	

	p_sem();

		if(p2shm->write_permission != NAO_PODE_ESCREVER){
			i=0;

			paux = pshm;
			while(paux[i].nreq != 0 ){
				i++;
			}

			paux[i].nreq = ++p2shm->last_nreq;
			strcpy(paux[i].max_time,aux.max_time);
			paux[i].num_proc = aux.num_proc;
			paux[i].start_time = aux.start_time;
			paux[i].status = aux.status;
		   	strcpy(paux[i].proc,aux.proc); 
		   	paux[i].pid = 0;
		}else{
			printf("Gerenciador em processo de desligamneto\n");
		}


	v_sem();

	fclose(fp);
	return 0;
}