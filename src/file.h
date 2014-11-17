/*
 * file.h
 *
 *  Created on: Nov 16, 2014
 *      Author: edmund
 */

#ifndef FILE_H
#define FILE_H

#include "rftp-messages.h"
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
FILE *create_dir_and_file (char *output_dir, char *filename);
int write_data_to_file(data_message *packet, FILE *target);
int get_filesize(FILE *file);
int check_fileread(FILE *file);
void show_transfer_info(char *filename, char *filesize, char *server_name);

#endif /* FILE_H_ */
