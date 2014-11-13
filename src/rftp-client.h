/*
 *  Name        : rftp-client.h
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong
 *  Description : Client-specific functions for the Reliable File Transfer Protocol (RFTP).
 *
 *  CS 3357a Assignment 2
 */

#ifndef RFTP_CLIENT_H
#define RFTP_CLIENT_H

#include "udp-sockets.h"

/*
 * Function prototypes.
 */
int rftp_transfer_file(char *server_name, char *port_number, char *filename, int timeout, int verbose);
int initialize_transfer (int sockfd, host_t *dest, int timeout, char *filename);

#endif /* RFTP_CLIENT_H */
