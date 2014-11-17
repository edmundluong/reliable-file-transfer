/*
 *  Name        : udp-server.c
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>, Jeff Shantz <jeff@csd.uwo.ca>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong, Jeff Shantz
 *  Date        : November 11, 2014
 *  Description : Implements a UDP server socket to listen on.
 *
 *  CS 3357a Assignment 2
 */

#include "udp-server.h"
#include "udp-sockets.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

/*
 * Binds a socket.
 *
 * Return a socket file description, if successful.
 * Exit the program if unable to bind the socket.
 */
int bind_socket (struct addrinfo *addr_list)
{
    struct addrinfo *addr;
    int sockfd;

    // Iterate through each addrinfo in the list; stop when we successfully bind
    // to one.
    for (addr = addr_list; addr != NULL; addr = addr->ai_next)
    {
        // Open a socket.
        sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

        // Try the next address if we couldn't open a socket.
        if (sockfd == -1)
            continue;

        // Try to bind the socket to the address/port.
        if (bind(sockfd, addr->ai_addr, addr->ai_addrlen) == -1)
        {
            // If binding fails, close the socket, and try the next address.
            close(sockfd);
            continue;
        }
        else
        {
            // Otherwise, we've bound the address/port to the socket, so stop
            // processing.
            break;
        }
    }

    // Free the memory allocated to the addrinfo list.
    freeaddrinfo(addr_list);

    // If addr is NULL, we tried every addrinfo and weren't able to bind to any.
    if (addr == NULL)
    {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Otherwise, return the socket descriptor.
        return sockfd;
    }

}

/*
 * Binds a socket as a UDP server, and listens on the specified port number.
 *
 * Return a UDP server socket file descriptor, if successful.
 * Exit the program if unable to bind the socket.
 */
int create_server_socket (char *port)
{
    struct addrinfo *results = get_udp_sockaddr(NULL, port, AI_PASSIVE);
    int sockfd = bind_socket(results);

    return sockfd;
}

