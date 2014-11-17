/*
 *  Name        : udp-sockets.h
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>, Jeff Shantz <jeff@csd.uwo.ca>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong, Jeff Shantz
 *  Date        : November 11, 2014
 *  Description : UDP socket implementation.
 *
 *  CS 3357a Assignment 2
 */

#ifndef UDP_SOCKETS_H
#define UDP_SOCKETS_H

#include <stdint.h>
#include <netdb.h>

/*
 * Used to store information of source addresses.
 */
typedef struct
{
    struct sockaddr_in addr;
    socklen_t addr_len;
    char friendly_ip[INET_ADDRSTRLEN];
} host_t;

/*
 * Function prototypes
 */
struct addrinfo *get_udp_sockaddr (const char *node, const char *port,
        int flags);

#endif /* UDP_SOCKETS_H */
