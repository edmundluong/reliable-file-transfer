/*
 *  Name        : rftp-client.c
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong
 *  Description : Client-specific functions for the Reliable File Transfer Protocol (RFTP).
 *
 *  CS 3357a Assignment 2
 */

#include "rftp-client.h"

/*
 * Transfers a file to the UDP server, via the Reliable File Transfer Protocol (RFTP).
 */
int rftp_file_transfer(char *server, char *port_number, char *filename, int timeout, int verbose)
{
    int result = 1;

    // Return status of the transfer.
    if (result)
        return 1;
    else
        return 0;
}
