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

/*
 * Function prototypes.
 */
int rftp_receive_file(char *port_number, char *output_dir, int time_wait, int verbose);
int initialize_transfer(int sockfd, host_t *source);

#endif /* RFTP_SERVER_H */
