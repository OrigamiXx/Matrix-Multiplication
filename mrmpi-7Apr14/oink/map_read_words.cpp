#include "string.h"
#include "keyvalue.h"
using namespace MAPREDUCE_NS;

#define MAXLINE 1024

/* ----------------------------------------------------------------------
   read_words
   read words from file, separated by whitespace
   output: key = word, value = NULL
------------------------------------------------------------------------- */

void read_words(int itask, char *file, KeyValue *kv, void *ptr)
{
  char line[MAXLINE];
  char *whitespace = " \t\n\f\r\0";

  int *nfiles = (int *) ptr;
  (*nfiles)++;

  FILE *fp = fopen(file,"r");
  while (fgets(line,MAXLINE,fp)) {
    char *word = strtok(line,whitespace);
    while (word) {
      kv->add(word,strlen(word)+1,NULL,0);
      word = strtok(NULL,whitespace);
    }
  }
  fclose(fp);
}
