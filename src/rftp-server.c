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
control_message *initialize_receive (int sockfd, host_t *source, int verbose)
{
    // Receive a control message from client.
    control_message *msg = (control_message*) receive_rftp_message(sockfd,
                                                                   source);
    if (msg)
    {
        // If the message is an initialization message.
        if (msg->type == 1 && ntohs(msg->seq_num) == 0)
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
 * Accepts a file from a client.
 */
int accept_file (int sockfd, host_t *source, char *filename, int filesize,
        char *output_dir, int time_wait, int verbose)
{
    control_message *control;   // A control message.
    data_message *data;         // A data message.
    int next_seq = 1;           // The next expected sequence number.

    // Receive data from the client until a termination message is sent.
    rftp_message *msg = receive_rftp_message(sockfd, source);
    while ((control = (control_message*) msg)->type != TERM_MSG)
    {
        data = (data_message*) msg;
        if (data->type == DATA_MSG && ntohs(data->seq_num) == next_seq)
        {
            // TODO: Code to write data into a file.

            // Update the next expected sequence number.
            next_seq = (next_seq == 1) ? 0 : 1;
        }

        // Receive another message from the client.
        msg = receive_rftp_message(sockfd, source);
    }
    // If a termination message was given, end the program.
    if (control->type == TERM_MSG)
    {
    }

    return 0;
}

/*
 * Receives a file from the UDP client, via the Reliable File Transfer Protocol (RFTP).
 */
int rftp_receive_file (char *port_number, char *output_dir, int time_wait,
        int verbose)
{
    host_t client;      // Client host.
    char *filename;     // Name of the file being transferred.
    int filesize;       // Size of the file being transferred.
    int status = 0;     // Status of the file transfer.

    // Create a socket and listen on port number.
    int sockfd = create_server_socket(port_number);
    printf("Listening on port %s\n", port_number);

    // If a file transfer was initialized.
    control_message *init = initialize_receive(sockfd, &client, verbose);
    if (init)
    {
        // Get the file's information from the init message.
        filesize = ntohl(init->fsize);
        filename = malloc(ntohl(init->fname_len) + 1);
        memcpy(filename, init->fname, ntohl(init->fname_len));

        // Receive the file from the client.
        printf("Receiving %s (%d B) from %s...\n", filename, filesize,
               client.friendly_ip);
        status = accept_file(sockfd, &client, filename, filesize, output_dir,
                             time_wait, verbose);

        // Free allocated memory and signal a successful file transfer.
        free(filename);
        free(init);
    }

    // Return status of the file transfer.
    return status;
}
