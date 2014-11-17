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
#include "rftp-messages.h"

/*
 * Function prototypes
 */
control_message *request_transfer_session (int sockfd, host_t *dest, char *filename,
        int timeout, int verbose);
int transfer_file (int sockfd, host_t *dest, char *filename, int filesize,
        int timeout, int verbose);
int end_transfer (int sockfd, host_t *dest, FILE *file, char *filename,
        int filesize, int next_seq, int timeout, int verbose);
int rftp_transfer_file (char *server_name, char *port_number, char *filename,
        int timeout, int verbose);

#endif /* RFTP_CLIENT_H */
