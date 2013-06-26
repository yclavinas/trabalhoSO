#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

void to_doidao()
{
   printf("recebi segment fault. Vou morrer!!!\n");
   exit(1);
}

main()
{
   char p[2];
   signal(SIGSEGV, to_doidao);
   
   /* vou forcar um segment fault */
   printf("%c", p[100000000	]);
}
