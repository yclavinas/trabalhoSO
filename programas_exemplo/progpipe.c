#include <stdio.h>
#include <stdlib.h>
#include <string.h>

main()
{
   int pid, status;
   int fd[2];
   char buf[30];

   if (( pipe(fd)) < 0)
   {
     printf("erro no pipe\n");
     exit(1);
   }
   if (( pid = fork()) < 0)
   {
     printf("erro no fork\n");
     exit(1);
   }
   if (pid == 0)
   {
     strcpy(buf, "mensagem do filho");
     write(fd[1], buf, sizeof(buf));
     exit(0);
   }
   read(fd[0], buf, sizeof(buf));
   printf("mensagem lida: %s\n",buf);
   wait(&status);
   exit(0);
}
     
