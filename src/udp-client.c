/*
 *  Name        : udp-client.c
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>, Jeff Shantz <jeff@csd.uwo.ca>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong, Jeff Shantz
 *  Date        : November 11, 2014
 *  Description : Implements a UDP client socket to listen on.
 *
 *  CS 3357a Assignment 2
 */

#include "udp-client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Creates a socket on the specified port number
 * for communications with a UDP server.
 *
 * Return a socket file descriptor, if successful.
 * Exit the program if a socket could not be binded.
 */
int create_client_socket (char *hostname, char *port, host_t *server)
{
    int sockfd;
    struct addrinfo *addr;
    struct addrinfo *results = get_udp_sockaddr(hostname, port, 0);

    // Iterate through the address information list for a socket.
    for (addr = results; addr != NULL; addr = addr->ai_next)
    {
        // Open a socket.
        sockfd = socket(addr->ai_family, addr->ai_socktype,
                        addr->ai_protocol);

        // Try the next address if socket could not be created.
        if (sockfd == -1) continue;

        // Store server address information and length.
        memcpy(&server->addr, addr->ai_addr, addr->ai_addrlen);
        memcpy(&server->addr_len, &addr->ai_addrlen,
               sizeof(addr->ai_addrlen));

        // Socket is created.
        break;
    }

    // Free the memory allocated to the address information list.
    freeaddrinfo(results);

    // If a socket could not be created, exit the program.
    if (addr == NULL)
    {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }
    // Return the socket file descriptor.
    else
    {
        return sockfd;
    }
}
