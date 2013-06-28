#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
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

typedef struct requisicao{
	int tempo_restante;
	int num_proc;
}REQUISICAO_T;


int max_proc,proc_livres;
int idsem;
struct sembuf operacao[1];
PROCESSO_T *pshm;
INFO_T *p2shm;

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

int str2sec(char *std_time){

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


void scheduler(){

	int i, nreq_escolhido=0;
	int max_time,start_time,current_time;
	char time_str[9];
	time_t time_now;
	REQUISICAO_T req;


	p_sem();

		if(proc_livres < max_proc){
			for(i=0;i<NUM_TAB;i++){
				if((pshm[i].nreq != 0)&&(pshm[i].status == PENDING)){

					max_time = str2sec(pshm[i].max_time);

					strftime(time_str,9,"%T",localtime(&pshm[i].start_time));
					start_time = str2sec(time_str);

					time_now = time(NULL);
					strftime(time_str,9,"%T",localtime(&time_now));
					current_time = str2sec(time_str);

					req.tempo_restante = max_time - (current_time - start_time);


					printf("Start: %d\nCurrent: %d\nMax: %d\nDif: %d\n\n\n",start_time,current_time,max_time,req.tempo_restante);

				}
			}

		}	

	v_sem();
}





int main(int argc,char* argv[]){

	int idshm,id2shm,idsem;


	signal(SIGALRM,scheduler);

	if(argc == 1){
		printf("Numero maximo de processos nao informado\n");
		exit(1);
	}

	max_proc = atoi(argv[1]);
	proc_livres = max_proc;


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


	while(1){
		alarm(1);
		sleep(2);
	}

	return 0;

}