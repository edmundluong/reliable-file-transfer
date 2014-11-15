/*
 *  Name        : rftp-server.h
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong
 *  Description : Server-specific functions for the Reliable File Transfer Protocol (RFTP).
 *
 *  CS 3357a Assignment 2
 */

#ifndef RFTP_SERVER_H
#define RFTP_SERVER_H

#include "udp-sockets.h"
#include "rftp-messages.h"

/*
 * Function prototypes.
 */
control_message *initialize_receive (int sockfd, host_t *source, int verbose);
int rftp_receive_file (char *port_number, char *output_dir, int time_wait,
        int verbose);
int receive_file (int sockfd, host_t *source, char *filename, int filesize,
        char *output_dir, int time_wait, int verbose);
int end_receive (int sockfd, host_t *source, control_message *term,
        FILE *target, int time_wait, int verbose);
FILE *create_dir_and_file (char *output_dir, char *filename);

#endif /* RFTP_SERVER_H */
