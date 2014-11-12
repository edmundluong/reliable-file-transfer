/*
 *  Name        : rftp-client.c
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong
 *  Description : Client-specific functions for the Reliable File Transfer Protocol (RFTP).
 *
 *  CS 3357a Assignment 2
 */

#include "rftp-client.h"
#include "rftp-messages.h"
#include "udp-sockets.h"
#include "udp-client.h"

/*
 * Transfers a file to the UDP server, via the Reliable File Transfer Protocol (RFTP).
 */
int rftp_file_transfer(char *server_name, char *port_number, char *filename, int timeout, int verbose)
{
    // Create a socket and listen on port number.
    host_t server;
    int sockfd = create_client_socket(server_name, port_number, &server);

    // If the transfer was initialized, begin transferring the file.
    if (initialize_transfer(sockfd, &server))
    {

    }

    // Return status of the transfer.
    if (1)
        return 1;
    else
        return 0;
}
