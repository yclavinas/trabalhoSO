#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>


#define NAO_PODE_ESCREVER -1

int idsem,id2sem;
struct sembuf operacao[1];

struct mensagem{
     long pid;
     int pid_process;
};

typedef struct info{
	int write_permission;
	int last_nreq;

}INFO_T;

int p_sem(){
     operacao[0].sem_num = 0;
     operacao[0].sem_op = 1;
     operacao[0].sem_flg = 0;
     if ( semop(idsem, operacao, 1) < 0)
       printf("erro no p=%d\n", errno);
}

int v_sem(){
     operacao[0].sem_num = 0;
     operacao[0].sem_op = -1;
     operacao[0].sem_flg = 0;
     if ( semop(idsem, operacao, 1) < 0)
       printf("erro no p=%d\n", errno);
}

/*ataches e destroies para remover os ICPS*/
int destroy_IPCS(int id2shm){
	int idshm;
	struct shmid_ds *buf;
	struct mensagem mensagem_rec;
	int idfila;

	/*Pegar o id para a area de mem compartilhada*/	
	if ((idshm = shmget(90108094, sizeof(int), 0x1ff)) < 0){
	    printf("erro na criacao da memoria compartilhada idshm\n");
	    exit(1);
	}
	
	/*da um msgget na fila para ter acesso */
	//if (idfila = msgget(90014255, 0x1ff) < 0){
	     //printf("erro no acesso fila\n");
	     //exit(1);
	//}
	
	shmctl(idshm,IPC_RMID, buf);
	shmctl(id2shm,IPC_RMID, buf);
	semctl(idsem,1,IPC_RMID, buf);
	semctl(id2sem,1,IPC_RMID, buf);
	
	/*Loop para acabar com os processos que estão em execucao, dando kill com o pid que pegarei da fila de msg*/
	//while(msgrcv(idfila, &mensagem_rec, sizeof(mensagem_rec)-sizeof(long), 0, IPC_NOWAIT) != NULL){
		//kill(mensagem_rec.pid_process, 9);
	//}
	
	//tem que dar ctl na fila
	
	printf("Todos mecanismos IPC foram removidos\n");
	//printf("Todos os processos terminaram\n");	
	return 0;
}

int main(){
	
	int id2shm, sucesso;
	INFO_T *p2shm;
	
	/*Em 2 min os IPCS serao destruídos, processos encerrados*/
	
	/*alarm(10);*/
	/*signal(SIGALRM, destroy_IPCS);*/
	
	/*Pegar o id para a area de mem compartilhada*/
	if ((id2shm = shmget(90108012, sizeof(INFO_T), 0x1ff)) < 0){
	    printf("erro na criacao da memoria compartilhada id2shm\n");
	    exit(1);
	}
	
	/*agora eu preciso bloquear o acesso.  Ou seja, preciso trocar p2shm para 1, ie NAO_PODE_ESCREVER*/
	p2shm = (INFO_T *) shmat(id2shm, (char *)0, 0);
	if (p2shm == (INFO_T *)-1){
        printf("erro no attach\n");
        exit(1);
    }

	/*da um semget em semaforo para ter acesso */
	if ((idsem = semget(90015266, 1, 0x1ff)) < 0){
	     printf("erro na criacao do semaforo\n");
	     exit(1);
	}

	/*da um semget no 2o semaforo para cria-lo*/
	if ((id2sem = semget(90015212, 1, IPC_CREAT|0x1ff)) < 0){
	     printf("erro na criacao do semaforo\n");
	     exit(1);
	}

	p_sem();
		/*vou bloquear o acesso, setando -1 em p2shm, pois p2shm jamais terá esse valor*/
		p2shm->write_permission = NAO_PODE_ESCREVER;
	v_sem();	
	
	sleep(4);	
	
	sucesso = destroy_IPCS(id2shm);
	if(sucesso == 0)
		printf("SO_shutdown finalizado com sucesso!\n");
	return(0);
}
