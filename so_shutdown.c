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

/*ataches e destroies para remover os ICPS*/
void destroy_IPCS(){
	int idsem, idshm;
	struct shmid_ds *buf;

	/*Pegar o id para a area de mem compartilhada*/	
	if ((idshm = shmget(90108094, sizeof(int), 0x1ff)) < 0){
	    printf("erro na criacao da memoria compartilhada\n");
	    exit(1);
	}
	
    /*da um semget em semaforo para ter acesso */
	if ((idsem = semget(90015266, 1, 0x1ff)) < 0){
	     printf("erro na criacao do semaforo\n");
	     exit(1);
	}
	
	shmctl(idshm,IPC_RMID, buf);
	semctl(idsem,1,IPC_RMID, buf);	
	
	printf("Pronto!");
	
}

int main(){
	
	int id2shm, *p2shm;
	
	/*Em 2 min os IPCS serao destruídos*/
	
	alarm(120);
	signal(SIGALRM, destroy_IPCS);

	//vou dar uma alarm(5). Quando estouram passo pro tratamento da interrupcao com as funcoes pra terminar.
	//fode com tudo e termina
	
	/*Pegar o id para a area de mem compartilhada*/
	if ((id2shm = shmget(90108094, sizeof(int), 0x1ff)) < 0){
	    printf("erro na criacao da memoria compartilhada\n");
	    exit(1);
	}
	
	/*agora eu preciso bloquear o acesso.  Ou seja, preciso trocar p2shm para 1, ie NAO_PODE_ESCREVER*/
	p2shm = (int *) shmat(id2shm, (char *)0, 0);
	if (p2shm == (int *)-1){
        printf("erro no attach\n");
        exit(1);
    }

	/*vou bloquear o acesso, setando -1 em p2shm, pois p2shm jamais terá esse valor*/
	*p2shm = NAO_PODE_ESCREVER;
	
	
	//kill(getpid(), SIGUSR1);
	
	return(0);
}