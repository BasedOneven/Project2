#include <stdio.h>
#include <stdlib.h>

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
    fclose(fp);
}