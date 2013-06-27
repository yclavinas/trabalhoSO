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

int main(){
	
	int idshm, id2shm, p2shm;
	int idsem;
	
	//vou bloquear o acesso, setando um em p2shm
	//vou dar uma alarm(5). Quando estouram passo pro tratamento da interrupcao com as funcoes pra terminar.
	//fode com tudo e termina
	
	
	SIGMASK();
	
	/*da um attach na mem compartilhada*/
	pshm = (struct processo *) shmat(idshm, (char *)0, 0);
	if (pshm == (struct processo *)-1) {
        printf("erro no attach\n");
        exit(1);
    }
	
	/*da um attach na mem compartilhada de bloqueio para ver se pode escrever ou nao*/
	p2shm = (int *) shmat(id2shm, (char *)0, 0);
	if (p2shm == (int *)-1){
        printf("erro no attach\n");
        exit(1);
    }

    /*da um semget em semaforo para ter acesso */
	if ((idsem = semget(90015266, 1, IPC_CREAT|0x1ff)) < 0){
	     printf("erro na criacao do semaforo\n");
	     exit(1);
	}
	
	return(0);
}