#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <assert.h>
#include "queue.h"

queue_t jobs;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m   = PTHREAD_MUTEX_INITIALIZER;

typedef struct __arg {
    char * addr;
    off_t offset;
} arg_t;

void *worker(void *arg);
int thread_pool_init(int n);

int main(int argc, char *argv[])
{
    // no files error
    char no_files_error_msg[30] = "wzip: file1 [file2 ...]\n";
    if(argc < 2)
    {
        printf("%s",no_files_error_msg);
        exit(1);
    }

    // combine all input files into a single file
    // create new file for combination of inputs 
    FILE *combinedFile = fopen("combined.txt", "w");

    // grab all files from the command line and put them in the combined file
    for(int i = 1; i < argc; i++) {
        char currentChar;
        FILE *fp = fopen(argv[i], "r");
        if(fp == NULL) {
            exit(1);
        }
        
        // store characters from the current file into the combined file
        while((currentChar = fgetc(fp)) != EOF)
        {
            fputc(currentChar, combinedFile);
        }
        fclose(fp);
    }
    fclose(combinedFile);

    // compress characters from the combined file
    char currentChar;
    int cur = 1;
    FILE *fp = fopen("combined.txt", "r");
    if(fp == NULL) {
        exit(1);
    }

    // check the size of the file in bytes
    struct stat st;
    off_t size;
    int fd = fileno(fp);
    fstat(fd, &st);
    size = st.st_size;

    // file to compress greate thatn 4096 bytes
    // Cannot fix seg faults at the current moment
    if (size < 0) 
    {
        pthread_t t[2];
        char *addr0 = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
        char *addr1 = (void*)((char*)addr0 + 80);
        char *addr2 = (void*)((char*)addr1 + 150);

        arg_t *argAddr0 = malloc(sizeof(arg_t));
        argAddr0->addr = addr0;
        argAddr0->offset = size;

        arg_t *argAddr1 = malloc(sizeof(arg_t));
        argAddr1->addr = addr1;
        argAddr1->offset = size;

        arg_t *argAddr2 = malloc(sizeof(arg_t));
        argAddr2->addr = addr2;
        argAddr2->offset = size;

        pthread_create(&t[0], NULL, worker, (void*) &argAddr0);
        pthread_create(&t[1], NULL, worker, (void*) &argAddr1);
        pthread_create(&t[2], NULL, worker, (void*) &argAddr2);

        pthread_join(t[0], NULL);
        pthread_join(t[1], NULL);
        pthread_join(t[2], NULL);
        
        munmap(addr0, size);
    }
    else
    {
        char tempChar = fgetc(fp);
        while(tempChar != EOF)
        {
            currentChar = fgetc(fp);
            if(tempChar == currentChar)
            {
                cur++;
            }
            else 
            {
                fwrite(&cur, 4, 1, stdout);
                fwrite(&tempChar, 1, 1, stdout);
                tempChar = currentChar;
                cur = 1;
            }
        }
    }

    fclose(fp);
}

void *worker(void *arg) {
    arg_t * argInfo = (arg_t *) arg;
    char * memInfo = argInfo->addr;
    int cur = 1;
    char tempChar = memInfo[0];
    char currentChar;

    for(int i = 0; i < argInfo->offset; i++)
    {
        currentChar = memInfo[i];
        if(tempChar == currentChar)
        {
            cur++;
        }
        else
        {
            fwrite(&cur, 4, 1, stdout);
            fwrite(&tempChar, 1, 1, stdout);
            tempChar = currentChar;
            cur = 1;
        }
    }

    return NULL;
}

int thread_pool_init(int n) {
  assert(n > 0);
  Queue_Init(&jobs);
  pthread_t t;

  for (int i = 1; i < n; i++)
    pthread_create(&t, NULL, worker, NULL);

  return 1;
}