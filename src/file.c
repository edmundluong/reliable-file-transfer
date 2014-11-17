/*
 * file.c
 *
 *  Created on: Nov 16, 2014
 *      Author: edmund
 */

#include "file.h"

#include <stdlib.h>
#include <stdio.h>

/*
 * Returns a file pointer, if the given file exists.
 *
 * Returns NULL if there was an error.
 */
FILE *get_file(char *filename)
{
    FILE *file = NULL;    // The specified file
    int fsize = NO_FSIZE; // The size of the file

    // Open the file, if it exists.
    if ((file = fopen(filename, "r")))
    {
        // If the file is has a valid size
        // and is under the maximum allowed size
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
                "file does not exist.\n", filename);
    }
    // If the filesize is a negative value, the file could be a directory or is corrupt.
    else if (fsize == NO_FSIZE)
    {
        printf("\nERROR: %s could not be sent, "
                "file is a directory or is corrupt.\n", filename);
    }
    // If the filesize exceeds the maximum size.
    else
    {
        printf("\nERROR: %s could not be sent, "
                "file exceeds the maximum allowed size (%d GB)", filename, (MAX_FSIZE/GB));
    }

    return NULL;
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
