//Authors: Kritik Patel and Manav Kumar

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>

#ifndef DEBUG
#define DEBUG 0
#endif
#ifndef DEBUG_WRAP
#define DEBUG_WRAP 0
#endif
#ifndef BUG_SIZE
#define BUF_SIZE 30
#endif

typedef struct {
    size_t length;
    size_t used;
    char *data;
} strbuf_t;

int sb_init(strbuf_t *L, size_t length)
{
    L->data = malloc(sizeof(char) * length);
    if (!L->data) return 1;

    L->length = length;
    L->used  = 1;
    L->data[0] = '\0';

    return 0;
}

void sb_destroy(strbuf_t *L)
{
    free(L->data);
}


int sb_append(strbuf_t *L, char item)
{
    if (L->used == L->length) {
        size_t size = L->length * 2;
        char *p = realloc(L->data, sizeof(char) * size);
        if (!p) return 1;
        
        L->data = p;
        L->length = size;

        if (DEBUG) printf("Increased size to %lu\n", size);
    }

    L->data[L->used-1] = item;
    L->data[L->used] = '\0';
    ++L->used;

    return 0;
}
int sb_concat(strbuf_t *sb, char *str){
    int i = 0;
    while(str[i] != '\0'){
        sb_append(sb,str[i]);
        i++;
    }
    return 0;
}

int wordWrap(int width, int fr, int fw){
    char *buf;                              //String buffer to hold the characters that we read from file
    buf = (char*) calloc(sizeof(char), BUF_SIZE);
    
    size_t nBytesread;
    strbuf_t word;
    sb_init(&word, width);
    int nByteswrite;
    int newLineTick = 0;
    bool isTooBig = false;
    int lTrack = 0;

    int whileIteration = 0; //For debugging

    while((nBytesread = read(fr, buf, BUF_SIZE)) > 0){    //Load buffer with file contents

        if(DEBUG_WRAP) printf("While loop iteration %d, buffer contains '%s'\n", whileIteration, buf);

        for(int i = 0; i < BUF_SIZE; i++){
            
            if(isspace(buf[i]) || buf[i] == '\0'){      //When the character is a space or has reached EOF

                if(word.used != 1){                     //If the word list is not empty, it'll be inserted in the output
                    if(newLineTick >= 2){               //Make a paragraph is there is one
                        write(fw, "\n", 1);
                        write(fw, "\n", 1);
                        if(DEBUG_WRAP) printf("Wrote paragraph at ");
                        lTrack = 0;
                    }
                    newLineTick = 0;

                    nByteswrite = word.used - 1;        //Bytes of the word to write
                    
                    if(width - lTrack < nByteswrite + 1){   //If the word doesn't fit on the current line, it will write to the next line
                        if(nByteswrite > width){    //Word is too big
                            if(DEBUG_WRAP) printf("Word is too big at ");
                            isTooBig = true;
                        }
                        write(fw, "\n", 1);
                        write(fw, word.data, nByteswrite);
                        lTrack = nByteswrite;

                        if(DEBUG_WRAP) printf("Step %d: lTrack = %d Wrote to next line '%s'\n", i, lTrack, word.data);

                        //Free and reinitizalize the word
                        sb_destroy(&word);
                        sb_init(&word, width);
                        
                    }
                    else{                               //The word fits and will be written on the same line

                        if(DEBUG_WRAP) printf("\nLINE TRACK, WIDTH, BYTESWRITE: %d, %d, %d\n", lTrack, width, nByteswrite);

                        if(lTrack != 0){
                            write(fw, " ", 1);
                            lTrack++;
                        }
                        write(fw, word.data, nByteswrite);
                        lTrack += nByteswrite;

                        if(DEBUG_WRAP) printf("Step %d: lTrack = %d Wrote to same line '%s'\n", i, lTrack, word.data);

                        //Free and reinitizalize the word
                        sb_destroy(&word);
                        sb_init(&word, width);
                    }
                    
                }
                else
                {
                    if(DEBUG_WRAP) printf("Step %d: Useless space\n", i);
                }
                
            }

            else{
                //means the word is in process of making, so keep adding it 
                sb_append(&word, buf[i]);

                if(DEBUG_WRAP) printf("Step %d: Appended %c\n", i, buf[i]);

            }

            if(buf[i] == '\n'){
                newLineTick++;
            }

            if(DEBUG_WRAP) printf("New lines tick: %d\n" , newLineTick);

            if(buf[i] == '\0')    //Passed EOF, break out of loop
                break;

        }
        free(buf);
        //buf = (char*)malloc(BUF_SIZE*sizeof(char));
        buf = (char*) calloc(sizeof(char), BUF_SIZE);
        whileIteration++;
    }

    //If we've reached EOF but there is still a word to add
    if(word.used != 1){                     //If the word list is not empty, it'll be inserted in the output
        if(newLineTick >= 2){               //Make a paragraph is there is one
            write(fw, "\n\n", 2);
            if(DEBUG_WRAP) printf("Wrote paragraph at ");
            lTrack = 0;
        }
        newLineTick = 0;

        nByteswrite = word.used - 1;        //Bytes of the word to write
        
        if(width - lTrack < nByteswrite + 1){   //If the word doesn't fit on the current line, it will write to the next line
            if(nByteswrite > width){    //Word is too big
                if(DEBUG_WRAP) printf("Word is too big at ");
                isTooBig = true;
            }
            write(fw, "\n", 1);
            write(fw, word.data, nByteswrite);
            lTrack = nByteswrite;

            if(DEBUG_WRAP) printf("Outside while: lTrack = %d Wrote to next line '%s'\n", lTrack, word.data);

            //Free and reinitizalize the word
            sb_destroy(&word);
            sb_init(&word, width);
            
        }
        else{                               //The word fits and will be written on the same line
            if(lTrack != 0){
                write(fw, " ", 1);
                lTrack++;
            }
            write(fw, word.data, nByteswrite);
            lTrack += nByteswrite;

            if(DEBUG_WRAP) printf("Outside while: lTrack = %d Wrote to same line '%s'\n", lTrack, word.data);

            //Free and reinitizalize the word
            sb_destroy(&word);
            sb_init(&word, width);
        }
        
    }
    else
    {
        if(DEBUG_WRAP) printf("Outside while: Useless space\n");
    }

    write(fw, "\n", 1);
    free(buf);
    sb_destroy(&word);

    if(isTooBig)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
int isdir(char *name){
    struct stat data;

	int err = stat(name, &data);
	// should confirm err == 0
	if(err){
	    perror("File does not exist or cannot be opened.\n");
		return EXIT_FAILURE;
    }

    if(S_ISDIR(data.st_mode))
        return 2;
    
    if(S_ISREG(data.st_mode))
        return 3;
    
    return EXIT_FAILURE;
}

int directoryAccess(char *dirName, int width){
    DIR *folder;
    struct dirent *entry;
    int returnStatus = EXIT_SUCCESS;

    folder = opendir(dirName);
    
    if(folder == NULL)
    {
        perror("Unable to read directory.\n");
        return(EXIT_FAILURE);
    }

    while((entry = readdir(folder)))
    {
        if(entry->d_name[0] == '.'){
            continue;
        }
        else if(entry->d_name[0] == 'w' && entry->d_name[1] == 'r' && entry->d_name[2] == 'a' && entry->d_name[3] == 'p' && entry->d_name[4] == '.'){
            continue;
        }
        else{
            strbuf_t fileNameIn;
            sb_init(&fileNameIn, 5);
            char *name = entry->d_name;
            char *slash = "/";
            sb_concat(&fileNameIn, dirName);
            sb_concat(&fileNameIn, slash);
            sb_concat(&fileNameIn, name);
            int fr = open(fileNameIn.data, O_RDONLY);
            //create the file name
            strbuf_t fileNameOut;
            sb_init(&fileNameOut, 5);
            char *pre = "wrap.";
            sb_concat(&fileNameOut, dirName);
            sb_concat(&fileNameOut, slash);
            sb_concat(&fileNameOut, pre);
            sb_concat(&fileNameOut, name);
            if(DEBUG) printf("File name in: %s\n", fileNameIn.data);
            if(isdir(fileNameIn.data) == 3){
                if(DEBUG) printf("Creating and writing to %s from %s \n", fileNameOut.data, fileNameIn.data);
                int fw = open(fileNameOut.data, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);

                if(returnStatus == 0)
                    returnStatus = wordWrap(width, fr, fw);
                else
                    wordWrap(width, fr, fw);
            }
            
            sb_destroy(&fileNameIn);
            sb_destroy(&fileNameOut);
        }
    }
    closedir(folder);
    return(returnStatus);
}

int main(int argc, char* argv[]){
    if(argc == 1){                                  //No arguments given
        if(DEBUG) printf("No arguments given.\n");
        return EXIT_FAILURE;
    }
    int width = atoi(argv[1]);
    if(width == 0){
        if(DEBUG) printf("No valid width given.\n");
        return EXIT_FAILURE;
    }
    if(argc == 2){                                  //Only width given, read from stdin and write to stdout
        if(DEBUG) printf("Only given width, will wrap stdin\n");
        return wordWrap(width, 0, 1);
    }

    int argType = isdir(argv[2]);
    if(argType == EXIT_FAILURE)                      //If the argument could not be found or opened
        return EXIT_FAILURE;

    else if(argType == 2){                //If the argument is a directory
        if(DEBUG) printf("Arg is directory\n");
        return directoryAccess(argv[2], width);
    }

    else if(argType == 3){                //If the argument is a regular file
        if(DEBUG) printf("Arg is file\n");
        int fr = open(argv[2], O_RDONLY);
        return wordWrap(width, fr, 1);
    }

    return EXIT_FAILURE;
}