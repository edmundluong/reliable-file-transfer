/*
 *  Name        : udp-client.h
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>, Jeff Shantz <jeff@csd.uwo.ca>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong, Jeff Shantz
 *  Date        : November 11, 2014
 *  Description : Implements a UDP client socket to listen on.
 *
 *  CS 3357a Assignment 2
 */

#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include "udp-sockets.h"

/*
 * Function prototypes
 */
int create_client_socket (char *hostname, char *port, host_t *server);

#endif /* UDP_CLIENT_H */
