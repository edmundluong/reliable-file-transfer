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
 * Binds a socket to the specified address and port.
 *
 * Return a socket file description, if successful.
 * Exit the program if unable to bind the socket.
 */
int bind_socket (struct addrinfo *addr_list)
{
    struct addrinfo *addr;
    int sockfd;

    // Iterate through the address information list for a socket.
    for (addr = addr_list; addr != NULL; addr = addr->ai_next)
    {
        // Open a socket.
        sockfd = socket(addr->ai_family, addr->ai_socktype,
                        addr->ai_protocol);

        // Try the next address if socket could not be created.
        if (sockfd == -1) continue;

        // Bind the socket to the address/port.
        if (bind(sockfd, addr->ai_addr, addr->ai_addrlen) == -1)
        {
            // If binding fails, close the socket, and try the next address.
            close(sockfd);
            continue;
        }
        // Socket has been binded.
        else
        {
            break;
        }
    }

    // Free the memory allocated to the address information list.
    freeaddrinfo(addr_list);

    // If a socket could not be binded, exit the program.
    if (addr == NULL)
    {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }
    // Return the socket file descriptor.
    else
    {
        return sockfd;
    }

}

/*
 * Binds a UDP server socket on the specified port number.
 *
 * Return a UDP server socket file descriptor, if successful.
 * Exit the program if unable to bind the socket.
 */
int create_server_socket (char *port)
{
    // Get the address information for the port number.
    struct addrinfo *results = get_udp_sockaddr(NULL, port, AI_PASSIVE);

    // Bind the socket, and get the socket file descriptor.
    int sockfd = bind_socket(results);

    // Return the socket descriptor.
    return sockfd;
}

