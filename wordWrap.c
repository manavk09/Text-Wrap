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
    L->data[L->used] = '\0';

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
    //the buf that will hold the lines that we read from file
    char *buf;
    buf = (char*)malloc(width*sizeof(char));
    int lTrack = 0;

    size_t nBytesread;
    //start reading the file 
    
    int whileIteration = 0;
    strbuf_t word;
    sb_init(&word, width);

    while((nBytesread = read(fr, buf, width)) != 0){
        //after buf is loaded with words start wrapping 
        if(DEBUG) printf("While loop iteration: %d\n", whileIteration);
        for(int i = 0; i < width; i++){
            size_t nByteswrite;
            //when the word is a space it is either the begining or the end of the word 
            if(buf[i] == '\0' && buf[i - 1] == '\0')
                break;
            if(isspace(buf[i]) || buf[i] == '\0'){
                //if the word list is empty that means it is start so it 
                if(word.used != 1){
                    //get the len of the word to write
                    nByteswrite = word.used - 1;
                    if(width - lTrack < nByteswrite){   //If word doesn't fit on current line
                        write(fw, "\n", 1);
                        write(fw, word.data, nByteswrite);
                        lTrack = nByteswrite;
                        if(DEBUG) printf("Step %d: Wrote to next line %s\n", i, word.data);
                        sb_destroy(&word);
                        sb_init(&word, width);
                    }
                    else{
                        //write the word to the file after end
                        if(lTrack != 0){
                            write(fw, " ", 1);
                            lTrack++;
                        }
                        write(fw, word.data, nByteswrite);
                        lTrack += nByteswrite;
                        if(DEBUG) printf("Step %d: Wrote to same line %s\n", i, word.data);
                        //free the word once ended
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
            
        }
        whileIteration++;
    }
    
    free(buf);
    return 1;

}

int main(int argc, char argv[]){
    int fr = open("readTesting.txt", O_RDONLY);
    int fw = open("writeTesting.txt", O_WRONLY);
    wordWrap(10, fr, fw);

}