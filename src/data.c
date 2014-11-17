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
