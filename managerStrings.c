//
// Created by benjaminsantana on 5/28/20.
//
#include "managerStrings.h"

void stripNewLine( char *str, size_t len)
{
    int i;


    for (  i = 0; i < len; ++i )
    {
        if ( str[i] == '\n' )
        {
            str[i] = '\0';

            return;
        }
    }

}

char *getCommand(char *str, size_t len){

    int i;

    char *cmdBuff;

    cmdBuff = NULL;
    cmdBuff = malloc(20*sizeof(char ));

    for(i=0; i<len; i++){

        if(str[i] == ' '){

            break;

        }

    }

    memcpy(cmdBuff, &str[0], i);
    cmdBuff[i] = '\0';

    return cmdBuff;


}


char* stringUntil(char* str, size_t len, char breakpoint){


    int i;

    char *cmdBuff;

    cmdBuff = NULL;
    cmdBuff = malloc(50*sizeof(char ));

    for(i=0; i<len; i++){

        if(str[i] == breakpoint){

            break;

        }

    }

    memcpy(cmdBuff, &str[0], i);
    cmdBuff[i] = '\0';

    return cmdBuff;

}


off_t fsize(const char *filename) {
    struct stat st;

    if (stat(filename, &st) == 0)
        return st.st_size;

    fprintf(stderr, "Cannot determine size of %s: %s\n",
            filename, strerror(errno));

    return -1;
}

