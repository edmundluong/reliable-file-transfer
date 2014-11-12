/*
 * rftp-server.c
 *
 *  Created on: Nov 11, 2014
 *      Author: edmund
 */

#include <stdio.h>
#include "rftp-messages.h"
#include "rftp-server.h"
#include "udp-sockets.h"
#include "udp-server.h"

/*
 * Initializes the server to begin receiving a file from a client.
 */
int initialize_receive(int sockfd, host_t *source)
{
    // Receive a control message from client.
    control_message *msg = (control_message*) receive_rftp_message(sockfd, source);

    // If the message is an initialization message, return 1.
    if (msg)
        if (msg->type == 1 && msg->seq_num == 0)
            return 1;

    // Else return 0.
    return 0;
}

/*
 * Receives a file from the UDP client, via the Reliable File Transfer Protocol (RFTP).
 */
int rftp_receive_file(char *port_number, char *output_dir, int time_wait, int verbose)
{
    // Create a socket and listen on port number.
    host_t client;
    int sockfd = create_server_socket(port_number);

    // If a file transfer was initialized.
    if (initialize_receive(sockfd, &client))
    {
        printf("File transfer initialized.\n");
    }

    // Return status of file transfer.
    if (1)
        return 1;
    else
        return 0;
}
