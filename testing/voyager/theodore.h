/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * theodore.h                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#ifndef THEODORE_H
#define THEODORE_H

#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "wrh5_defs.h"

#define HDRFLD_INT(tag, tagstr) \
    if(strcmp(key, tagstr) == 0) { \
        header.tag = atoi(value); \
        printf("\t%s:\t%s -> %d\n", tagstr, value, header.tag); \
    }

#define HDRFLD_DBL(tag, tagstr) \
    if(strcmp(key, tagstr) == 0) { \
        header.tag = atof(value); \
        printf("\t%s:\t%s -> %f\n", tagstr, value, header.tag); \
    }

#define HDRFLD_STR(tag, tagstr) \
    if(strcmp(key, tagstr) == 0) { \
        strcpy(header.tag, value); \
        printf("\t%s:\t%s -> %s\n", tagstr, value, header.tag); \
    }

#endif
