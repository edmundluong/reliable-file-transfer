/*
 * rftp-server.c
 *
 *  Created on: Nov 11, 2014
 *      Author: edmund
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rftp-messages.h"
#include "rftp-server.h"
#include "udp-sockets.h"
#include "udp-server.h"

/*
 * Initializes the server to begin receiving a file from a client.
 */
control_message *initialize_receive(int sockfd, host_t *source)
{
    // Receive a control message from client.
    control_message *msg = (control_message*) receive_rftp_message(sockfd, source);
    if (msg)
    {
        // If the message is an initialization message.
        if (msg->type == 1 && msg->seq_num == 0)
        {
            // Acknowledge the message and send it back to the client.
            msg->ack = 1;
            send_rftp_message(sockfd, (rftp_message*) msg, source);

            // Return the message.
            return msg;
        }
    }

    // If there was an error with the init message, return 0.
    free(msg);
    return NULL;
}

/*
 * Receives a file from the UDP client, via the Reliable File Transfer Protocol (RFTP).
 */
int rftp_receive_file(char *port_number, char *output_dir, int time_wait, int verbose)
{
    host_t client;              // Client host.

    // Create a socket and listen on port number.
    int sockfd = create_server_socket(port_number);
    printf("Listening on port %s\n", port_number);

    // If a file transfer was initialized.
    control_message *init = initialize_receive(sockfd, &client);
    if (init)
    {
        printf("File transfer initialized.\n");
        printf("control message:\n");
        printf("msg->type: %d\n", init->type);
        printf("msg->ack: %d\n", init->ack);
        printf("msg->seq: %d\n", init->seq_num);
        printf("msg->size: %d\n", init->fsize);
        printf("msg->fname_len: %d\n", init->fname_len);
        printf("msg->fname: %s\n", init->fname);

        return 1;
    }

    // If transfer fails, return 0.
    return 0;
}
