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
control_message *initialize_transfer (int sockfd, host_t *dest, char *filename,
        int timeout, int verbose)
{
    // Construct an initialization message.
    rftp_message *init;
    if ((init = create_init_message(filename)))
    {
        // Send init message to server using Stop-and-Wait.
        if (stop_and_wait_send(sockfd, dest, init, INIT_MSG, timeout, verbose))
        {
            return (control_message*) init;
        }
    }

    // Transfer failed to initialize, return NULL.
    free(init);
    return NULL;
}

/*
 * Sends a RFTP message, and waits for an acknowledgment from the server.
 * Returns 1 if message was sent and acknowledged.
 * Returns 0 if an error occurred while sending the message.
 */
int stop_and_wait_send (int sockfd, host_t* dest, rftp_message *msg,
        int msg_type, int timeout, int verbose)
{
    rftp_message *response = NULL;      // The RFTP response message from the server.
    control_message *control = NULL;    // A RFTP control message.
    data_message *data = NULL;          // A RFTP data message.
    int result = 0;                     // The result of the operation.

    // Send the message to the server.
    int retval = send_rftp_message(sockfd, dest, msg);
    while (retval != -1)
    {
        // Listen for an acknowledgment of the packet.
        if ((response = receive_rftp_message_with_timeout(sockfd, dest, timeout)))
        {
            // Handle control message acknowledgments.
            if (msg_type == INIT_MSG || msg_type == TERM_MSG)
            {
                // Compare original message with response.
                control = (control_message*) response;
                control_message *orig = (control_message*) msg;
                if ((control->type == orig->type)
                        && (ntohs(control->seq_num) == ntohs(orig->seq_num))
                        && (control->ack == ACK))
                {
                    result = 1;
                    response = NULL;
                    break;
                }
            }
            // Handle data message acknowledgments.
            else
            {
                // Compare original message with response.
                data = (data_message*) response;
                data_message *orig = (data_message*) msg;
                if ((data->type == orig->type)
                        && (ntohs(data->seq_num) == ntohs(orig->seq_num))
                        && (data->ack == ACK))
                {
                    result = 1;
                    data = NULL;
                    break;
                }
            }
        }
        // Send message again when timed out.
        retval = send_rftp_message(sockfd, dest, msg);
    }
    // Free allocated memory and return the result of the operation.
    if (response)
        free(response);
    if (control)
        free(control);
    if (data)
        free(data);
    return result;
}

int end_transfer (int sockfd, host_t *dest, FILE *file, char *filename,
        int filesize, int next_seq, int timeout, int verbose)
{
    int result = 0;     // The result of the termination.
    rftp_message *term; // A termination message.

    // Close the file and create a termination message.
    fclose(file);
    if ((term = create_term_message(next_seq, filename, filesize)))
    {
        // Send termination message to server using Stop-and-Wait.
        if (stop_and_wait_send(sockfd, dest, term, TERM_MSG, timeout, verbose))
            result = 1;
    }

    // If an error occurred, return 0.
    free(term);
    return result;
}

/*
 * Transfer a file to the server.
 */
int transfer_file (int sockfd, host_t *dest, char *filename, int filesize,
        int timeout, int verbose)
{
    FILE *file;                         // The file to be transferred.
    rftp_message *packet = NULL;        // Packet of file data to be transferred.
    uint8_t buffer[DATA_MSS];           // Buffer to hold data.
    int next_seq = 1;                   // The next sequence number.
    int status = 1;                     // The status of the file transfer.

    // Open the file to be transferred.
    if ((file = fopen(filename, "rb")))
    {
        // While the end of the file has not been reached.
        int bytes_read;
        while (!feof(file))
        {
            // Read data from the file.
            bytes_read = fread(buffer, sizeof(uint8_t), sizeof(buffer), file);
            // If there was a read error, output the error and exit the program.
            if (ferror(file))
            {
                perror("File read error.\n");
                status = 0;
                break;
            }
            // Create a data packet and send it to the server.
            else
            {
                // Create a data packet and send it to the server using Stop-and-Wait.
                if ((packet = create_data_message(next_seq, bytes_read, buffer)))
                {
                    // If the packet is acknowledged, update the sequence number.
                    if (stop_and_wait_send(sockfd, dest, packet, DATA_MSG,
                                           timeout, verbose))
                    {
                        next_seq = (next_seq == 1) ? 0 : 1;
                    }
                    // An error occurred while sending the packet.
                    else
                    {
                        status = 0;
                        break;
                    }
                }
            }
        }
        // End the file transfer.
        status = end_transfer(sockfd, dest, file, filename, filesize, next_seq,
                              timeout, verbose);
    }

    // Free allocated memory and return the status of the file transfer.
    free(packet);
    return status;
}

/*
 * Transfers a file to the UDP server, via the Reliable File Transfer Protocol (RFTP).
 */
int rftp_transfer_file (char *server_name, char *port_number, char *filename,
        int timeout, int verbose)
{
    host_t server;                      // Server host.
    control_message *init = NULL;       // Initialization message.
    int filesize;                       // The size of the file being transferred.
    int status = 0;                     // Status of the file transfer.

    // Create a socket and listen on port number.
    int sockfd = create_client_socket(server_name, port_number, &server);
    printf("Trying to initiate a file transfer with %s:%s ...\n", server_name,
           port_number);

    // If the transfer was initialized, begin transferring the file.
    if ((init = initialize_transfer(sockfd, &server, filename, timeout, verbose)))
    {
        // Get the filesize of the file transfer.
        filesize = ntohl(init->fsize);
        printf("File transfer initialized.\n\n");

        // Transfer the file to the server.
        printf("Sending %s (%d B) to %s ...\n", filename, filesize,
               server_name);
        status = transfer_file(sockfd, &server, filename, filesize, timeout,
                               verbose);
    }

    // Return the status of the file transfer.
    free(init);
    return status;
}
