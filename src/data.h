/*
 *  Name        : data.h
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong
 *  Date        : November 16, 2014
 *  Description : Implementation of various data-related functions and helper methods.
 *
 *  CS 3357a Assignment 2
 */

#ifndef DATA_H
#define DATA_H

/*
 * Data-oriented macros
 */
#define kB 1000       // Kilobyte
#define MB 1000000    // Megabyte
#define GB 1000000000 // Gigabyte

/*
 * Function prototypes
 */
int is_byte(int filesize);
int is_kilobyte(int filesize);
int is_megabyte(int filesize);
int bytes_to_kilo (int filesize);
int bytes_to_mega (int filesize);
int bytes_to_giga (int filesize);
double bytes_to_dkilo (int filesize);
double bytes_to_dmega (int filesize);
double bytes_to_dgiga (int filesize);

#endif /* DATA_H_ */
