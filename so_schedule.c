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
}PROCESSO_T;

typedef struct info{
	int write_permission;
	int last_nreq;

}INFO_T;

int idsem;
struct sembuf operacao[1];

int p_sem()
{
     operacao[0].sem_num = 0;
     operacao[0].sem_op = 1;
     operacao[0].sem_flg = 0;
     if ( semop(idsem, operacao, 1) < 0)
       printf("erro no p=%d\n", errno);
}
int v_sem()
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





int main(int argc,char* argv[]){


	int max_proc,processos_running;
	int idshm, id2shm;
	INFO_T *p2shm;
	PROCESSO_T *pshm,*paux;
	struct tm * timeinfo;
	char start_time[50];
	int i;


	if(argc == 1){
		printf("Numero maximo de processos nao informado\n");
		exit(-1);
	}

	max_proc = atoi(argv[1]);
	processos_running = 0;

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

	/*da um shmget na mem compartilhada de bloqueio*/	
	if ((id2shm = shmget(90108012, sizeof(INFO_T), IPC_CREAT|0x1ff)) < 0){
	    printf("erro na criacao da memoria compartilhada\n");
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


    /*da um semget em semaforo para ter acesso */
	if ((idsem = semget(90015266, 1, IPC_CREAT|0x1ff)) < 0){
	     printf("erro na criacao do semaforo\n");
	     exit(1);
	}

	p_sem();

	for (i = 0; i < NUM_TAB; i++)
	{

		timeinfo = localtime(&pshm[i].start_time);
		strftime(start_time,50,"%X",timeinfo);

		printf("%s\n",start_time);
	}

	v_sem();












	return 0;
}