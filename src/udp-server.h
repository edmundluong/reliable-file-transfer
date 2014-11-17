/*
 *  Name        : udp-server.h
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>, Jeff Shantz <jeff@csd.uwo.ca>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong, Jeff Shantz
 *  Date        : November 11, 2014
 *  Description : Implements a UDP server socket to listen on.
 *
 *  CS 3357a Assignment 2
 */

#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <netdb.h>

/*
 * Function declarations.
 */
int bind_socket (struct addrinfo *addr_list);
int create_server_socket (char *port);

#endif /* UDP_SERVER_H */
