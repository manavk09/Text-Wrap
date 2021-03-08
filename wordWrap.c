#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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

int wordWrap(int width, int fd, int fw){
    //the buf that will hold the lines that we read from file
    char  *buf;
    buf = (char*)malloc(width*sizeof(char));

    size_t nBytesread;
    size_t bytesRead = width;
    //start reading the file 
    while((nBytesread = read(fd, buf, bytesRead))!= -1){
        //after buf is loaded with words start wrapping 
        for(int i = 0; i < width; i++){
            strbuf_t word;
            sb_init(word, width);
            size_t nByteswrite;
            //keeps track of the 
            int lTrack = 0;
            //when the word is a space it is either the begining or the end of the word 
            if(isspace(buf[i]){
                //if the word list is empty that means it is start so it 
                if(word.used != 1){
                    //get the len of the word to write
                    nByteswrite = word.used - 1;
                    write(fw, " ", 1);
                    lTrack++;
                    //write the word to the file after end
                    write(fw, word, nByteswrite);
                    //free the word once eneded
                    sb_destroy(&word);
                }
                else
                {
                    //do nothing
                }
                
            }
            else{
                //means the word is in process of making, so keep adding it 
                sb_append(&word, buf[i]);
                lTrack++;
            }
            
        }
    }

}