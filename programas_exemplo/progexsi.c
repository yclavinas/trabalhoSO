#include <signal.h>
#include <errno.h>
#include <stdio.h>

void funcao_sigusr1()
{
   printf("recebi ALRM\n");
}
void funcao_sigusr2()
{
   printf("recebi sigusr2\n");
}

main()
{
   alarm(2);
   signal(SIGALRM, funcao_sigusr1);
   signal(SIGUSR2, funcao_sigusr2);

   sleep(10);
   printf("%s\n","Sei la\n" );

   //kill(getpid(), SIGUSR1);
   //kill(getpid(), SIGUSR2);

//sleep(10);
}
