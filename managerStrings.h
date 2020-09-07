//
// Created by benjaminsantana on 5/28/20.
//

#ifndef FILESYSTEM_MANAGERSTRINGS_H
#define FILESYSTEM_MANAGERSTRINGS_H
#include "managerStrings.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void stripNewLine(char*, size_t);
char *getCommand(char*, size_t);
char* stringUntil(char*, size_t,char);
off_t fsize(const char*);

#endif //FILESYSTEM_MANAGERSTRINGS_H
