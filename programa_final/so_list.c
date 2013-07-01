/**

Cain‹ Felipe Bento Razzolini - 09/0108094
Yuri Cossich Lavinas - 09/0015266

Compilador: gcc 4.6.3 
	x86_64-Linux-gnu
 
SO: Ubuntu 12.04 LTS 64 bits

Algoritmo de escalonamento: FIFO
	O escalonador ir‡ buscar um arquivo na tabela de arquivos com o tempo de entrada mais antigo, caracterizando o FIFO. S— ser‡
executado um arquivo por vez, mesmo que o max_proc n‹o esteja todo ocupado.

Mecanismos PIC: 
	Foram usados 2 tipos de mecanismos IPC, sem‡foros e mem—ria compartilhada. E para cada tipo, foram usadas duas inst‰ncias.
	
		1.As mem—rias compartilhadas foi utilizada para termos acesso a tabela e vari‡veis de controle de acesso e escrita.
	Em idshm foi criada para conter a tabela de processos, enquanto id2shm foi criada para, qudndo so_shutdown for executado, podermos
	evitar que novos processos possam ser ou executados ou entrarem na tabela de processos.

		2.J‡ os sem‡foros foram utilizados para garantir que tenhamos somente um processo com acesso a dados/vari‡veis vitais.	
	idsem foi criado para garantir acesso œnico as mem—rias compartilhadas. id2sem, para a quantidade de processos que podem ser alocados.

As estruturas de dados:
	INFO_T, PROCESSO_T, INFO_T, PROG_T


	PROCESSO_T: Estrutura da tabela de processos
	INFO_T: Estrutura contento o contador de requisicoes e flag de termino
	PROG_T: Informaçoes do programa






*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define RUNNING 1
#define PENDING 0
#define NUM_TAB 50
#define NAO_PODE_ESCREVER -1


typedef struct processo{
	int nreq;
	char max_time[9];
	int num_proc;
	time_t start_time;
	int status;
	char proc[50];
	int pid;/*pid do processo dispatcher*/
}PROCESSO_T;

int idsem;
struct sembuf operacao[1];

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

char* getStatusString(int status){

	if(status == PENDING){
		return "PENDING";
	}else{
		return "RUNNING";
	}

}

void getTotalTime(time_t start_time,char* totaltime){

	time_t current_time;
	int total_time,hr,min,sec;

	current_time = time(NULL);

	total_time = (int)difftime(current_time,start_time);

	hr = total_time/3600;
	min = (total_time%3600)/60;
	sec = ((total_time%3600)%60);

	sprintf(totaltime,"%02d:%02d:%02d",hr,min,sec);
}

int main(){


	int idshm;
	PROCESSO_T *pshm;
	char totaltime[10];
	int i;


	/*da um shmget na mem compartilhada*/	
	if ((idshm = shmget(90108094, NUM_TAB*sizeof(struct processo), IPC_CREAT|0x1ff)) < 0){
	    printf("erro na criacao da memoria compartilhada\n");
	    exit(1);
	}

	/*da um attach na mem compartilhada*/
	pshm = (struct processo *) shmat(idshm, (char *)0, 0);
	if (pshm == (struct processo *)-1) {
        printf("erro no attach\n");
        exit(1);
    }
	
	/*da um semget em semaforo para cria-lo*/
	if ((idsem = semget(90015266, 1, IPC_CREAT|0x1ff)) < 0){
	     printf("erro na criacao do semaforo\n");
	     exit(1);
	}


    /*da um semget em semaforo para ter acesso */
	if ((idsem = semget(90015266, 1, IPC_CREAT|0x1ff)) < 0){
	     printf("erro na criacao do semaforo\n");
	     exit(1);
	}

	totaltime[0] = 0;
	printf("%-4s\t%9s\t%-6s\t%-9s\t%-8s\t%-s\n","Nreq","Max_time","N_proc","Time","Status","Program");
	p_sem();
		for(i=0;i<NUM_TAB;i++){
			if(pshm[i].nreq > 0){
				getTotalTime(pshm[i].start_time,totaltime);
				printf("%-4d\t%9s\t%-6d\t%-9s\t%-8s\t%-s\n",pshm[i].nreq,pshm[i].max_time,pshm[i].num_proc,
				totaltime,getStatusString(pshm[i].status),pshm[i].proc);
			}
		}

	v_sem();
	return 0;

}

