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

/*typedef struct requisicao{
	int tempo_restante;
	int num_proc;
}REQUISICAO_T;*/


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
int idsem;
struct sembuf operacao[1];
REQ_PIDS_T pids[500];
PROCESSO_T *pshm;
INFO_T *p2shm;


int z;



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


}




void scheduler_FIFO(){

	int i,j,k,l,m,nreqs_num,nreqs_escolhidos[NUM_TAB],done,pid,status;
	PROG_T prog;

	


	for(i=0;i<NUM_TAB;i++){
		nreqs_escolhidos[i] = 0;

	}

	i = 0;
	nreqs_num = 0;

	p_sem();
		while((proc_livres > 0)&&(i<NUM_TAB)){
			if((pshm[i].nreq != 0)&&(pshm[i].num_proc <= proc_livres)&&(pshm[i].status == PENDING)){

				proc_livres -= pshm[i].num_proc;
				nreqs_escolhidos[nreqs_num] = pshm[i].nreq;
				nreqs_num++;
			}
			i++;
		}


		done = 0;

		j = 0;
		for(i=0;((i<NUM_TAB)&&(j<nreqs_num)&&(!done));i++){
			if(pshm[i].nreq == nreqs_escolhidos[j]){
				pshm[i].status = RUNNING;
				prog.n_params = countParams(pshm[i].proc);
				getArgs(pshm[i],&prog);

				pid = fork();
				if(pid < 0){
					printf("Erro no fork\n");
					exit(-1);
				}else if(!pid){

					for(k=0;k<pshm[i].num_proc;k++){
						pid = fork();

						if(pid < 0){
							printf("Erro no fork\n");
							exit(-1);
						}else if(!pid){
							l=0;

							/*Nesse trecho pode estar ocorrendo condicao de corrida*/
							while(pids[l].pid == 0){
								l++;
							}
							pids[l].nreq = pshm[i].nreq;
							pids[l].pid = getpid();
							/*Nesse trecho pode estar ocorrendo condicao de corrida*/

							execv(prog.params[0],prog.params);
						}else{
							if(k == pshm[k].num_proc - 1){
								for(m=0;m<pshm[k].num_proc;m++){
									wait(&status);
								}



								for(m=0;m<500;m++){
									if(pids[m].nreq == pshm[i].nreq){
										printf("\nPid: %d - Proc = %d\n",pids[m].nreq,pshm[i].nreq);
										kill(pids[m].pid,SIGKILL);
										pids[m].nreq = 0;
										pids[m].pid = 0;
									}
								}
								
								/*Condicao de corrida*/
								proc_livres += pshm[i].num_proc;
								/*Condicao de corrida*/
								pshm[i].nreq = 0;

								exit(1);
							}
						}
					}


				}
				j++;
			}
			if(j == nreqs_num){
				done = 1;
			}


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

	/*for(z=0;z<NUM_TAB;z++){
		pshm[z].status = PENDING;

	}exit(1);*/


	while(1){
		alarm(1);
		sleep(2);
	}

	return 0;
}
