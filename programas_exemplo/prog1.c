#include <stdio.h>
#include <stdlib.h>

main()
{
   int pid;

   if (( pid = fork()) < 0)
   {
     printf("erro no fork\n");
     exit(1);
   }
   if (pid == 0)
     printf("sou o processo filho do prog1\n");
   else
   {
	//printf("%d \n", pid);
     printf("sou o processo pai do prog1\n");
     sleep(10);
	printf("saiu do sleep");
   }
}
     
