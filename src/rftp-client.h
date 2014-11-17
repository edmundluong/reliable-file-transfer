/*
 *  Name        : rftp-client.h
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong
 *  Date        : November 11, 2014
 *  Description : Implementation of a Reliable File Transfer Protocol (RFTP) client,
 *                used to transfer a file to a RFTP server.
 *
 *  CS 3357a Assignment 2
 */

#ifndef RFTP_CLIENT_H
#define RFTP_CLIENT_H

#include "rftp-messages.h"
#include "udp-sockets.h"

/*
 * Function prototypes
 */
control_message *request_transfer_session (int sockfd, host_t *dest, char *filename,
        int timeout, int verbose);
int transfer_file (int sockfd, host_t *dest, char *filename, int filesize,
        int timeout, int verbose);
int end_transfer_session (int sockfd, host_t *dest, char *filename,
        int filesize, int next_seq, int timeout, int verbose);
int rftp_transfer_file (char *server_name, char *port_number, char *filename,
        int timeout, int verbose);

#endif /* RFTP_CLIENT_H */
