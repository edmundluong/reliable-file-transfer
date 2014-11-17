/*
 *  Name        : data.c
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong
 *  Date        : November 16, 2014
 *  Description : Implementation of various data-related functions.
 *
 *  CS 3357a Assignment 2
 */

#include "data.h"

/*
 * Returns how many digits there are in a filesize.
 *
 * Used for calculating the necessary padding for progress output.
 */
int get_num_digits (int filesize) {
    if (filesize < 0) return 0;
    if (filesize < 10) return 1;
    if (filesize < 100) return 2;
    if (filesize < 1000) return 3;
    if (filesize < 10000) return 4;
    if (filesize < 100000) return 5;
    if (filesize < 1000000) return 6;
    if (filesize < 10000000) return 7;
    if (filesize < 100000000) return 8;
    if (filesize < 1000000000) return 9;

    // Unlikely, but just in case.
    return 10;
}

/*
 * Returns whether the filesize is in bytes.
 */
int is_byte (int filesize)
{
    return (filesize < kB);
}

/*
 * Returns whether the filesize is in kilobytes.
 */
int is_kilobyte (int filesize)
{
    return ((filesize >= kB) && (filesize < MB));
}

/*
 * Returns whether the filesize is in megabytes.
 */
int is_megabyte (int filesize)
{
    return (filesize >= MB);
}

/*
 * Converts bytes to kilobytes.
 */
int bytes_to_kilo (int filesize)
{
    return (filesize / kB);
}

/*
 * Converts bytes to megabytes.
 */
int bytes_to_mega (int filesize)
{
    return (filesize / MB);
}

/*
 * Converts bytes to gigabytes.
 */
int bytes_to_giga (int filesize)
{
    return (filesize / GB);
}

/*
 * Converts bytes to kilobytes, as a double.
 */
double bytes_to_dkilo (int filesize)
{
    return ((double) filesize / (double) kB);
}

/*
 * Converts bytes to megabytes, as a double.
 */
double bytes_to_dmega (int filesize)
{
    return ((double) filesize / (double) MB);
}

/*
 * Converts bytes to gigabytes, as a double.
 */
double bytes_to_dgiga (int filesize)
{
    return ((double) filesize / (double) GB);
}
