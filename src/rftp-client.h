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
 * Function prototypes.
 */
control_message *initialize_transfer (int sockfd, host_t *dest, char *filename,
        int timeout, int verbose);
int rftp_transfer_file (char *server_name, char *port_number, char *filename,
        int timeout, int verbose);
int transfer_file (int sockfd, host_t *dest, char *filename, int filesize,
        int timeout, int verbose);
int end_transfer (int sockfd, host_t *dest, FILE *file, char *filename,
        int filesize, int next_seq, int timeout, int verbose);
int stop_and_wait_send (int sockfd, host_t* dest, rftp_message *msg,
        int msg_type, int timeout, int verbose);
int output_sent_progress (int bytes_sent, int total_bytes, int last_mult);

#endif /* RFTP_CLIENT_H */
