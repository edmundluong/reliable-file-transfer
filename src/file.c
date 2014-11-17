/*
 *  Name        : file.c
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong
 *  Date        : November 16, 2014
 *  Description : Implementation of various file-related functions.
 *
 *  CS 3357a Assignment 2
 */

#include "file.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

/*
 * Opens the given file, with the specified flag.
 *
 * Returns a file pointer to the file, if it exists.
 * Returns NULL if there was an error.
 */
FILE *get_file (char *filename, char *flag)
{
    FILE *file = NULL;    // The specified file
    int fsize = NO_FSIZE; // The size of the file

    // Open the file, if it exists.
    if ((file = fopen(filename, flag)))
    {
        // If the file is has a valid size
        // and is under the maximum allowed size.
        fsize = get_filesize(file);
        if ((fsize != NO_FSIZE) && (fsize <= MAX_FSIZE))
        {
            return file;
        }
    }
    // If the file does not exist.
    if (!file)
    {
        printf("\nERROR: %s could not be sent, "
               "file does not exist.\n",
               filename);
    }
    // If the filesize is a negative value,
    // the file could be a directory or is corrupt.
    else if (fsize == NO_FSIZE)
    {
        printf("\nERROR: %s could not be sent, "
               "file is a directory or is corrupt.\n",
               filename);
    }
    // If the filesize exceeds the maximum size.
    else
    {
        printf("\nERROR: %s could not be sent, "
               "file exceeds the maximum allowed size (%d GB)",
               filename, (MAX_FSIZE / GB));
    }

    return NULL;
}

/*
 * Creates the specified directory, and creates a file in that directory.
 */
FILE *create_dir_and_file (char *output_dir, char *filename)
{
    // Create the full pathname.
    char* path = malloc(strlen(output_dir) + strlen(filename) + 2);
    if (path)
    {
        path[0] = 0;
        strcat(path, output_dir);
        strcat(path, "/");
        strcat(path, filename);
    }

    // Create the directory and return the file pointer.
    mkdir(output_dir, 0700);
    FILE* file = fopen(path, "wb");
    free(path);

    return file;
}

/*
 * Returns the number of bytes in a file.
 */
int get_filesize (FILE *file)
{
    int size = NO_FSIZE; // Size of the file

    // Seek to the end of the file.
    fseek(file, 0, SEEK_END);
    // Get the number of bytes in the file.
    size = ftell(file);
    // Seek back to the start of the file.
    fseek(file, 0, SEEK_SET);

    return size;
}

/*
 * Checks if there was a file read error.
 *
 * Returns 1 if there was no read error with the selected file.
 * Returns 0 if there was an error, and outputs the error message.
 */
int check_fileread (FILE *file)
{
    // If a file error occurred.
    if (ferror(file))
    {
        perror("File read error: ");
        return FILE_ERROR;
    }

    return NO_ERROR;
}
