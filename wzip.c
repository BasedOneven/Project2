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
    if (size > 4096) 
    {
        // create threads
        thread_pool_init(3);
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
  while (1) {	
    pthread_mutex_lock(&m);
    while (Queue_Empty(&jobs))// if empty, no job to do
	{
		pthread_cond_wait(&fill, &m); // wait for jobs
	}

    int fd;
    Queue_Dequeue(&jobs, &fd);
    pthread_mutex_unlock(&m);

    // request_handle(fd);
    // close_or_die(fd);
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