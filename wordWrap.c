#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef DEBUG
#define DEBUG 1
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

int wordWrap(int width, int fr, int fw){
    char *buf;                              //String buffer to hold the characters that we read from file
    buf = (char*)malloc(width*sizeof(char));
    int lTrack = 0;

    size_t nBytesread;
    int whileIteration = 0;
    strbuf_t word;
    sb_init(&word, width);

    while((nBytesread = read(fr, buf, width)) > 0){    //Loads buffer with file contents, start wrapping 
        if(DEBUG) printf("While loop iteration: %d\n", whileIteration);
        for(int i = 0; i < width; i++){
            size_t nByteswrite;

            if(isspace(buf[i]) || buf[i] == '\0'){      //When the character is a space or has reached EOF
                if(word.used != 1){                     //If the word list is not empty, it'll be inserted in the output
                    //get the len of the word to write
                    nByteswrite = word.used - 1;
                    if(width - lTrack < nByteswrite){   //If the word doesn't fit on the current line
                        write(fw, "\n", 1);
                        write(fw, word.data, nByteswrite);
                        lTrack = nByteswrite;
                        if(DEBUG) printf("Step %d: lTrack = %d Wrote to next line %s\n", i, lTrack, word.data);

                        //Free and reinitizalize the word
                        sb_destroy(&word);
                        sb_init(&word, width);
                    }
                    else{
                        if(DEBUG) printf("\nLINE TRACK, WIDTH, BYTESWRITE: %d, %d, %d\n", lTrack, width, nByteswrite);
                        if(lTrack != 0){
                            write(fw, " ", 1);
                            lTrack++;
                        }
                        write(fw, word.data, nByteswrite);
                        lTrack += nByteswrite;
                        if(DEBUG) printf("Step %d: lTrack = %d Wrote to same line %s\n", i, lTrack, word.data);

                        //Free and reinitizalize the word
                        sb_destroy(&word);
                        sb_init(&word, width);
                    }
                    
                }
                else
                {
                    if(DEBUG) printf("Step %d: Useless space\n", i);
                }
                
            }
            else{
                //means the word is in process of making, so keep adding it 
                sb_append(&word, buf[i]);
                if(DEBUG) printf("Step %d: Appended %c\n", i, buf[i]);
            }
            
            if(buf[i] == '\0')    //Passed EOF, break out of loop
                break;
            
        }
        free(buf);
        buf = (char*)malloc(width*sizeof(char));

        whileIteration++;
    }
    
    free(buf);
    return 1;

}

int main(int argc, char argv[]){
    int fr = open("readTesting.txt", O_RDONLY);
    int fw = open("writeTesting.txt", O_WRONLY);
    wordWrap(30, fr, fw);

}