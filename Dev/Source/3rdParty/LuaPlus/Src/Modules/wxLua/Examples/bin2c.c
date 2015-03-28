/*
* bin2c.c
* convert binary files to byte arrays
* Luiz Henrique de Figueiredo (lhf@tecgraf.puc-rio.br)
* 11 Sep 2000 22:37:14
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

void dump(FILE* f, const char *p)
{
 int n;
 printf("static const unsigned char %s[]={\n", p);
 for (n=1;;n++)
 {
  int c=getc(f); 
  if (c==EOF) break;
  printf("%3u,",c);
  if (n==20) { putchar('\n'); n=0; }
 }
 printf("\n};\n\n");
}

void fdump(char* fn, const char *n)
{
 FILE* f= (fn==NULL) ? stdin : fopen(fn,"rb");	/* must open in binary mode */
 if (f==NULL)
 {
  fprintf(stderr,"bin2c: cannot open ");
  perror(fn);
  exit(1);
 }
 else
 {
  if (fn!=NULL) printf("/* %s */\n",fn);
  dump(f,n);
  fclose(f);
 }
}

int main(int argc, char* argv[])
{
 if (argc>2)
 {
  fdump(argv[1],argv[2]);
 }
 return 0;
}
