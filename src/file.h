/*
 *  Name        : file.h
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong
 *  Date        : November 16, 2014
 *  Description : Implementation of various file-related functions.
 *
 *  CS 3357a Assignment 2
 */

#ifndef FILE_H
#define FILE_H

#include "data.h"

#include <stdio.h>

#define FILE_ERROR 0
#define NO_ERROR 1

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
int get_filesize(FILE *file);
int check_fileread(FILE *file);
void show_transfer_info(char *filename, char *filesize, char *server_name);

#endif /* FILE_H */
