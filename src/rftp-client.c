/*
 *  Name        : rftp-client.c
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong
 *  Description : Client-specific functions for the Reliable File Transfer Protocol (RFTP).
 *
 *  CS 3357a Assignment 2
 */

#include <stdlib.h>
#include "rftp-client.h"
#include "rftp-messages.h"
#include "udp-sockets.h"
#include "udp-client.h"

/*
 * Attempts to initialize a file transfer to a UDP server. When the server does not
 * acknowledge an initialization request, another request will be sent when it timeouts.
 */
int initialize_transfer (int sockfd, host_t *dest, int timeout, char *filename)
{
    // Construct an initialization message.
    rftp_message *init = create_init_message(filename);
    if (init)
    {
        // Print statements to check before send.
        control_message *msg = (control_message*) init;
        printf("control message:\n");
        printf("msg->type: %d\n", msg->type);
        printf("msg->ack: %d\n", msg->ack);
        printf("msg->seq: %d\n", msg->seq_num);
        printf("msg->fsize: %d\n", msg->fsize);
        printf("msg->fname_len: %d\n", msg->fname_len);
        printf("msg->fname: %s\n", msg->fname);

        // Send the initialization message to the server.
        control_message *response;
        int retval = send_rftp_message(sockfd, init, dest);
        while (retval != -1)
        {
            // Listen for an acknowledgment from the server of the initialization.
            if ((response =
                    (control_message*) receive_rftp_message_with_timeout(
                            sockfd, timeout, dest)))
            {
                // If the request was acknowledged, return 1.
                if (response->type == 1 && response->ack == 1)
                {
                    free(init);
                    free(response);
                    return 1;
                }
            }

            // Send the initialization message again when timed out.
            retval = send_rftp_message(sockfd, (rftp_message*) init, dest);
        }
        free(response);
    }

    // Transfer failed to initialize, return 0.
    free(init);
    return 0;
}

/*
 * Transfers a file to the UDP server, via the Reliable File Transfer Protocol (RFTP).
 */
int rftp_transfer_file (char *server_name, char *port_number, char *filename,
        int timeout, int verbose)
{
    // Create a socket and listen on port number.
    host_t server;
    int sockfd = create_client_socket(server_name, port_number, &server);

    // If the transfer was initialized, begin transferring the file.
    if (initialize_transfer(sockfd, &server, timeout, filename))
    {
        printf("Transfer of %s initialized.\n", filename);
        return 1;
    }

    // If the transfer failed to send, return 0.
    return 0;
}
