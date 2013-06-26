#include<errno.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct mensagem
  {
     long pid;
     char msg[30];
  };

struct mensagem mensagem_env, mensagem_rec;

main()
{
   int pid, idfila, fd[2], estado;
  
   /* cria */										  /*  oug oug  oug*/
   if ((idfila = msgget(0x1223, IPC_CREAT|0x1B6)) < 0)/*  110 110 110*/
   {					/* 0001 0010 0010 0011 */
     printf("erro na criacao da fila\n");
     exit(1);
   }

   pid = fork();

   if (pid == 0)
   {
   mensagem_env.pid = getpid();
   sleep(5);
   strcpy(mensagem_env.msg, "Eu sou muito louco!! \n");
   msgsnd(idfila, &mensagem_env, sizeof(mensagem_env)-sizeof(long), 0);
   exit (0);
   }

   msgrcv(idfila, &mensagem_rec, sizeof(mensagem_rec)-sizeof(long), 0, 0);
   printf("mensagem recebida = %ld %s\n", mensagem_rec.pid, mensagem_rec.msg);
   wait (&estado);
   exit(0);
}
     
