/*
 *  Name        : udp-sockets.c
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>, Jeff Shantz <jeff@csd.uwo.ca>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong, Jeff Shantz
 *  Date        : November 11, 2014
 *  Description : UDP socket implementation.
 *
 *  CS 3357a Assignment 2
 */

#include "udp-sockets.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Gets UDP socket information for the given port number.
 *
 * Return address information of a service provider.
 */
struct addrinfo *get_udp_sockaddr (const char *node, const char *port, int flags)
{
    struct addrinfo hints;
    struct addrinfo *results;
    int retval;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family   = AF_INET;    // Return socket addresses for our local IPv4 addresses
    hints.ai_socktype = SOCK_DGRAM; // Return UDP socket addresses
    hints.ai_flags    = flags;      // Socket addresses should be listening sockets

    retval = getaddrinfo(node, port, &hints, &results);

    if (retval != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(retval));
        exit(EXIT_FAILURE);
    }

    return results;
}

