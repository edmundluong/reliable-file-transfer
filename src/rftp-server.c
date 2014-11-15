/*
 * rftp-server.c
 *
 *  Created on: Nov 11, 2014
 *      Author: edmund
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "rftp-messages.h"
#include "rftp-server.h"
#include "udp-sockets.h"
#include "udp-server.h"

/*
 * Helper method to create the output directory and target file.
 */
FILE *create_dir_and_file (char *output_dir, char *filename)
{
    // Create the full pathname
    char* path = malloc(strlen(output_dir) + strlen(filename) + 2);
    if (path)
    {
        strcat(path, output_dir);
        strcat(path, "/");
        strcat(path, filename);
    }
    // Create the directory and return the file pointer.
    mkdir(output_dir, 0700);
    FILE* file = fopen(path, "wb");
    printf("full path: %s\n", path);
    free(path);
    return file;
}

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
        if (msg->type == INIT_MSG && ntohs(msg->seq_num) == 0)
        {
            // Acknowledge the message and send it back to the client.
            msg->ack = ACK;
            int retval = send_rftp_message(sockfd, source, (rftp_message*) msg);

            // Return the message.
            if (retval != -1)
                return msg;
        }
    }

    // If there was an error with the init message, return 0.
    free(msg);
    return NULL;
}

/*
 * Terminates the file transfer.
 */
int end_receive (int sockfd, host_t *source, control_message *term,
        FILE *target, int time_wait, int verbose)
{
    int result = 1; // Result of the termination.

    // Close the target file.
    fclose(target);

    // Acknowledge the termination message and send it back to client.
    term->ack = ACK;
    int retval = send_rftp_message(sockfd, source,  (rftp_message*) term);

    // Go into a waiting state for any duplicate termination requests.
    control_message *dupe;
    while ((dupe = (control_message*) receive_rftp_message_with_timeout(
            sockfd, source, time_wait)) && retval != -1)
    {
        // Resend termination acknowledgment.
        dupe->ack = ACK;
        retval = send_rftp_message(sockfd, source, (rftp_message*) dupe);
    }
    // If any messages fail to send, signal an error.
    if (retval == -1)
        result = 0;

    // Return the result of the termination.
    free(dupe);
    return result;
}

/*
 * Accepts a file from a client.
 */
int receive_file (int sockfd, host_t *source, char *filename, int filesize,
        char *output_dir, int time_wait, int verbose)
{
    control_message *term;      // A termination message.
    data_message *data;         // A data message.
    FILE *target;               // The target file.
    int next_seq = 1;           // The next expected sequence number.
    int status = 1;             // The status of the file transfer.

    // Create the output directory and output file.
    target = create_dir_and_file(output_dir, filename);
    // Receive data from the client until a termination message is sent.
    rftp_message *msg = receive_rftp_message(sockfd, source);
    while ((term = (control_message*) msg)->type != TERM_MSG)
    {
        // If the received data packet is not a duplicate, write the data to file.
        data = (data_message*) msg;
        if (data->type == DATA_MSG && ntohs(data->seq_num) == next_seq)
        {
            // Write data to file.
            fwrite(data->data, sizeof(uint8_t), ntohl(data->data_len), target);
            if (ferror(target))
            {
                perror("File write error.\n");
                fclose(target);
                status = 0;
                break;
            }

            // Acknowledge the data packet and send back to client.
            data->ack = ACK;
            send_rftp_message(sockfd, source, (rftp_message*) data);

            // Update the next expected sequence number.
            next_seq = (next_seq == 1) ? 0 : 1;
        }
        // Receive another message from the client.
        msg = receive_rftp_message(sockfd, source);
    }
    // If a termination message was given, end the file transfer.
    if (term->type == TERM_MSG)
    {
        status = end_receive(sockfd, source, term, target, time_wait, verbose);
    }
    // Free allocated memory and return the status of the file transfer.
    free(msg);
    free(term);
    free(data);
    return status;
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
    printf("Listening on port %s...\n", port_number);

    // If a file transfer was initialized.
    control_message *init = initialize_receive(sockfd, &client, verbose);
    if (init)
    {
        // Get the file's information from the init message.
        filesize = ntohl(init->fsize);
        filename = malloc(ntohl(init->fname_len) + 1);
        memcpy(filename, init->fname, ntohl(init->fname_len));

        // Receive the file from the client.
        printf("File transfer initialized.\n\n");
        printf("Receiving %s (%d B) from %s into %s ...\n", filename, filesize,
               client.friendly_ip, output_dir);
        status = receive_file(sockfd, &client, filename, filesize, output_dir,
                              time_wait, verbose);

        // Free allocated memory and signal a successful file transfer.
        free(filename);
        free(init);
    }

    // Return status of the file transfer.
    return status;
}
