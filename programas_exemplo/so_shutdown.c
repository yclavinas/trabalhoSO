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
#include <signal.h>

#define NAO_PODE_ESCREVER -1

/*ataches e destroies para remover os ICPS*/
void destroy_IPCS{
	
	/*da um attach na mem compartilhada*/
	pshm = (struct processo *) shmat(idshm, (char *)0, 0);
	if (pshm == (struct processo *)-1) {
        printf("erro no attach\n");
        exit(1);
    }
	
	

    /*da um semget em semaforo para ter acesso */
	if ((idsem = semget(90015266, 1, IPC_CREAT|0x1ff)) < 0){
	     printf("erro na criacao do semaforo\n");
	     exit(1);
	}
	
}

int main(){
	
	int idshm, id2shm, p2shm;
	int idsem;
	

	//vou dar uma alarm(5). Quando estouram passo pro tratamento da interrupcao com as funcoes pra terminar.
	//fode com tudo e termina
	
	/*agora eu preciso bloquear o acesso.  Ou seja, preciso trocar p2shm para 1. Ele */
	p2shm = (int *) shmat(id2shm, (char *)0, 0);
	if (p2shm == (int *)-1){
        printf("erro no attach\n");
        exit(1);
    }

	/*vou bloquear o acesso, setando -1 em p2shm, pois p2shm jamais terá esse valor*/
	*pshm = NAO_PODE_ESCREVER;
	
	signal(SIGUSR1, funcao_sigusr1);
	kill(getpid(), SIGUSR1);
	
	
	
	
	
	return(0);
}