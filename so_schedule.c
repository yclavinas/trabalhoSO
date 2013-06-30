#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define RUNNING 1
#define PENDING 0
#define FINISHED -1
#define NUM_TAB 50
#define NAO_PODE_ESCREVER -1
#define MAX_PROCS 500


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


typedef struct prog{
	int n_params;
	char nome[50];
	char **params;
}PROG_T;

typedef struct req_pids{
	int nreq;
	int pid;
}REQ_PIDS_T;

int max_proc,proc_livres;
int idsem,id2sem;
struct sembuf operacao[1],operacao2[1];
REQ_PIDS_T pids[MAX_PROCS];
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

int p_sem2()
{
     operacao2[0].sem_num = 0;
     operacao2[0].sem_op = 1;
     operacao2[0].sem_flg = 0;
     if ( semop(id2sem, operacao2, 1) < 0)
       printf("erro no p=%d\n", errno);
}
int v_sem2()
{
     operacao2[0].sem_num = 0;
     operacao2[0].sem_op = -1;
     operacao2[0].sem_flg = 0;
     if ( semop(id2sem, operacao2, 1) < 0)
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


int countParams(char* input){
	int i,n_params;


	n_params = 1;/*O nome do programa sempre estara presente*/
	for(i=0;i<strlen(input);i++){
		if(input[i] == ' '){
			n_params++;
		}

	}

	return n_params;
}

void getArgs(PROCESSO_T procs,PROG_T *prog){

	int i,j,k;

	(*prog).params = (char**)calloc((*prog).n_params+1,sizeof(char*));

	k=0;
	for(i=0;i<(*prog).n_params;i++){
		j=0;
		(*prog).params[i] = (char*)calloc(50,sizeof(char));
		while((procs.proc[k] != ' ') && (procs.proc[k] != '\n') && (procs.proc[k] != '\0')){
			(*prog).params[i][j] = procs.proc[k];
			j++;
			k++;
		}
		(*prog).params[i][j] = 0;
		j++;
		k++;
		
	}
	(*prog).params[i] = (char*)0;
	strcpy(prog->nome,prog->params[0]);


}

void dbg(int val,char* str){

	if(str == NULL){
		printf("%d\n",val);
	}else{
		printf("%s\n",str);
	}
	exit(1);
}


void scheduler_FIFO(){

	int i,j,k,pid,status;
	int * processes_running;
	PROG_T prog;


	i = 0;

	p_sem();
		for(i=0;i<NUM_TAB;i++){
			if((pshm[i].nreq < 0)&&(pshm[i].status == FINISHED)){
				pshm[i].nreq = 0;
				p_sem2();
					/*printf("Liberando antes: %d\n",proc_livres);*/
					proc_livres += pshm[i].num_proc;
					/*printf("Liberando depois: %d\n",proc_livres);*/
				v_sem2();
			}
		}
	v_sem();

	i = 0;

	p_sem();

	while((proc_livres > 0)&&(i < NUM_TAB)){
		
		if((pshm[i].nreq > 0)&&(pshm[i].status == PENDING)&&(pshm[i].num_proc <= proc_livres)){
			
			p_sem2();
			/*printf("Alocando antes: %d\n",proc_livres);*/
			proc_livres -= pshm[i].num_proc;
			/*printf("Alocando depois: %d\n",proc_livres);*/
			v_sem2();

			
			prog.n_params = countParams(pshm[i].proc);
			getArgs(pshm[i],&prog);
			pshm[i].status = RUNNING;

			pid = fork();
			if(pid<0){
				printf("Erro no fork dispatcher\n");
				exit(-1);
			}else if(!pid){
				/*Processo dispatcher*/
				processes_running = (int*)calloc(pshm[i].num_proc,sizeof(int));

				for(j=0;j<pshm[i].num_proc;j++){

					processes_running[j] = fork();
					if(processes_running[j] < 0){
						printf("Erro no fork worker\n");
						exit(-1);
					}else if(processes_running[j] == 0){
						/*Processos worker*/

						execv(prog.nome,prog.params);
					}else{
						/*Processo dispatcher*/

						for(k=0;k<pshm[i].num_proc;k++){
							wait(&status);
							if((k == pshm[i].num_proc-1)&&(j==pshm[i].num_proc-1)){
								pshm[i].status = FINISHED;
								pshm[i].nreq = -1;

								exit(0);

							}
						}
					}
				}
			}
		}
		i++;
	}
	v_sem();

}




int main(int argc,char* argv[]){

	int idshm,id2shm;


	signal(SIGALRM,scheduler_FIFO);

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


	/*da um semget no 2o semaforo para cria-lo*/
	if ((id2sem = semget(90015212, 1, IPC_CREAT|0x1ff)) < 0){
	     printf("erro na criacao do semaforo\n");
	     exit(1);
	}


	while(p2shm->write_permission != NAO_PODE_ESCREVER){
		alarm(1);
		sleep(2);
	}

	return 0;
}


