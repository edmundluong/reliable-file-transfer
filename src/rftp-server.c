/*
 * rftp-server.c
 *
 *  Created on: Nov 11, 2014
 *      Author: edmund
 */

#include "rftp-messages.h"
#include "rftp-protocol.h"
#include "rftp-config.h"
#include "rftp-server.h"
#include "udp-sockets.h"
#include "udp-server.h"
#include "file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/*
 * Initializes the server to begin receiving a file from a RFTP client
 * when a file transfer initialization message is received.
 * The server acknowledges the initialization message, and begins receiving a file.
 *
 * Return an initialization message containing file transfer information, if successful.
 * Return NULL if there was an error with acknowledging the message.
 */
control_message *initialize_receive (int sockfd, host_t *source, int verbose)
{
    control_message *msg; // RFTP control message

    // Receive a control message from client.
    if ((msg = (control_message*) receive_rftp_message(sockfd, source,
                                                       verbose)))
    {
        // If the message is an initialization message.
        if (msg->type == INIT_MSG && ntohs(msg->seq_num) == 0)
        {
            // Acknowledge the message and return the initialization message.
            if (acknowledge_message(sockfd, source, (rftp_message*) msg,
                                    INIT_MSG, verbose))
            {
                return msg;
            }
        }
    }

    // If there was an error with acknowledging the message.
    free(msg);
    return NULL;
}

/*
 * Accepts data packets from a RFTP client, and writes them to a file.
 *
 * Return a successful status if the file was successfully received.
 * Return a failure status if the file failed to transfer.
 */
int receive_file (int sockfd, host_t *source, char *filename, int filesize,
        char *output_dir, int time_wait, int verbose)
{
    control_message *term = NULL; // RFTP termination message
    data_message *data = NULL;    // RFTP data message
    FILE *target = NULL;          // Target file
    int status = FAILURE;         // Status of the file transfer
    int next_seq = 1;             // Next expected sequence number
    int bytes_recv = 0;           // Total number of bytes received
    int curr_mult = 0;          // The current percent multiple being returned
    int last_mult = OUTPUTTED;    // Last outputted progress multiple
    int retval = SEND_ERR;        // The status of the send operations

    // Create the output directory and output file.
    target = create_dir_and_file(output_dir, filename);

    // Receive data from the client until a termination message is sent.
    rftp_message *msg = receive_rftp_message(sockfd, source, verbose);
    while (((term = (control_message*) msg)->type != TERM_MSG)
            && (retval != SEND_ERR))
    {
        // If the received data packet is not a duplicate, write the data to file.
        data = (data_message*) msg;
        if (data->type == DATA_MSG && ntohs(data->seq_num) == next_seq)
        {
            // Write data to file, if there was an error, close the file
            // and stop receiving data.
            if (!write_data_to_file(data, target))
            {
                fclose(target);
                break;
            }

            // Acknowledge the data packet and send back to client.
            retval = acknowledge_message(sockfd, source, (rftp_message*) data,
                                         DATA_MSG, verbose);

            // Give an output of received data.
            bytes_recv += ntohl(data->data_len);
            curr_mult = output_progress(RECV, bytes_recv, filesize,
                                        last_mult);
            if (curr_mult != OUTPUTTED) last_mult = curr_mult;

            // Update the next expected sequence number.
            next_seq = (next_seq == 1) ? 0 : 1;
        }

        // Receive another message from the client.
        free(msg);
        msg = receive_rftp_message(sockfd, source, verbose);
    }
    // If a termination message was given, end the file transfer.
    if (term->type == TERM_MSG)
    {
        fclose(target);
        term = NULL;
        status = end_receive_session(sockfd, source, term, target, time_wait,
                                     verbose);
    }

    // Free allocated memory and return the status of the file transfer.
    free(msg);
    return status;
}

/*
 * Terminates the file transfer session.
 *
 * Return a successful status if the file transfer session was terminated.
 * Return a failure status if there was an error sending an acknowledgment.
 */
int end_receive_session (int sockfd, host_t *source, control_message *term,
        FILE *target, int time_wait, int verbose)
{
    control_message *dupe; // Duplicate RFTP termination message
    int retval = SEND_ERR; // The result of the send operation

    // Acknowledge the termination message and send it back to client.
    retval = acknowledge_message(sockfd, source, (rftp_message*) term,
                                 term->type, verbose);

    // Go into a waiting state for any duplicate termination requests.
    while ((dupe = (control_message*) receive_rftp_message_with_timeout(
            sockfd, source, time_wait, verbose)) && (retval != SEND_ERR))
    {
        // Resend termination acknowledgment.
        retval = acknowledge_message(sockfd, source, (rftp_message*) dupe,
                                     dupe->type, verbose);
        free(dupe);
    }

    // Return the status of the termination.
    if (retval == SEND_ERR) return FAILURE;
    else return SUCCESS;
}

/*
 * Receives a file from a RFTP client, via the Reliable File Transfer Protocol (RFTP).
 *
 * Return a successful status if the file was successfully transferred.
 * Return a failure status if the file could not be transferred.
 */
int rftp_receive_file (char *port_number, char *output_dir, int time_wait,
        int verbose)
{
    host_t client;           // Client host
    char *filename = NULL;   // Name of the file being transferred
    int filesize = NO_FSIZE; // Size of the file being transferred
    int status = 0;          // Status of the file transfer

    // Create a socket and listen on port number.
    int sockfd = create_server_socket(port_number);
    printf("Listening on port %s for a file transfer request ...\n",
           port_number);

    // If a file transfer was initialized.
    control_message *init = initialize_receive(sockfd, &client, verbose);
    if (init)
    {
        // Get the file's information from the init message.
        printf("File transfer initialized.\n");
        printf("File will be transferred into %s.\n\n", output_dir);
        filesize = ntohl(init->fsize);
        filename = malloc(ntohl(init->fname_len) + 1);
        memcpy(filename, init->fname, ntohl(init->fname_len));
        filename[ntohl(init->fname_len)] = '\0';

        // Display the file transfer information.
        output_transfer_info(RECV, filename, filesize);

        // Receive the file from the client.
        status = receive_file(sockfd, &client, filename, filesize, output_dir,
                              time_wait, verbose);

        // Report the status of the file transfer.
        if (status)
        {
            // Success.
            printf("\n%s was successfully received from %s into %s.\n",
                   filename, client.friendly_ip, output_dir);
        }
        else
        {
            // Failure.
            printf("\nCould not successfully receive %s from %s.\n", filename,
                   client.friendly_ip);
        }
    }

    // Return status of the file transfer.
    close(sockfd);
    free(filename);
    free(init);
    return status;
}
