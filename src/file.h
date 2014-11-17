/*
 * file.h
 *
 *  Created on: Nov 16, 2014
 *      Author: edmund
 */

#ifndef FILE_H
#define FILE_H

#include "data.h"

#include <stdio.h>

/*
 * File-oriented macros
 */
#define MAX_FSIZE 2*GB // Maximum allowed filesize
#define NO_FSIZE -1    // An absence of filesize

/*
 * Function prototypes
 */
FILE *get_file(char *filename, char *flag);
int get_filesize(FILE *file);
int check_fileread(FILE *file);
void show_transfer_info(char *filename, char *filesize, char *server_name);

#endif /* FILE_H_ */
