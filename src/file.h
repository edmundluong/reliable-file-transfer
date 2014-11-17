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

#define MAX_FSIZE 2*GB // Maximum allowed filesize
#define NO_FSIZE -1    // An absence of filesize

FILE *get_file(char *filename);
int get_filesize(FILE *file);

#endif /* FILE_H_ */
