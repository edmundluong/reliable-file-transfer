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

#define OUTPUTTED -1
#define PASS 1
#define FAIL 0

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
    rftp_message *response = NULL;      // The RFTP response message from the server
    control_message *control = NULL;    // A RFTP control message
    data_message *data = NULL;          // A RFTP data message
    int result = FAIL;                  // The result of the operation

    // Send the message to the server.
    int retval = send_rftp_message(sockfd, dest, msg);
    if (verbose) verbose_msg_output(SENT, msg_type, msg);
    while (retval != -1)
    {
        // Listen for an acknowledgment of the packet.
        if ((response = receive_rftp_message_with_timeout(sockfd, dest, timeout)))
        {
            // Handle control message acknowledgments.
            if (verbose) verbose_msg_output(RECV, msg_type, response);
            if (msg_type == INIT_MSG || msg_type == TERM_MSG)
            {
                // Compare original message with response.
                control = (control_message*) response;
                control_message *orig = (control_message*) msg;
                if ((control->type == orig->type)  && (control->ack == ACK)
                        && (ntohs(control->seq_num) == ntohs(orig->seq_num)))
                {
                    result = PASS;
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
                if ((data->type == orig->type) && (data->ack == ACK)
                        && (ntohs(data->seq_num) == ntohs(orig->seq_num)))
                {
                    result = PASS;
                    data = NULL;
                    break;
                }
            }
        }
        // Send message again when timed out.
        retval = send_rftp_message(sockfd, dest, msg);
        if (verbose) verbose_msg_output(SENT, msg_type, msg);
    }
    // Free allocated memory and return the result of the operation.
    if (response) free(response);
    if (control) free(control);
    if (data) free(data);
    return result;
}

int end_transfer (int sockfd, host_t *dest, FILE *file, char *filename,
        int filesize, int next_seq, int timeout, int verbose)
{
    int result = FAIL;          // The result of the termination
    rftp_message *term;         // A termination message

    // Close the file and create a termination message.
    fclose(file);
    if ((term = create_term_message(next_seq, filename, filesize)))
    {
        // Send termination message to server using Stop-and-Wait.
        if (stop_and_wait_send(sockfd, dest, term, TERM_MSG, timeout, verbose)) result = PASS;
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
    FILE *file;                         // The file to be sent
    rftp_message *packet = NULL;        // Packet of file data to be sent
    uint8_t buffer[DATA_MSS];           // Buffer to hold data
    int next_seq = 1;                   // The next sequence number
    int status = PASS;                  // The status of the file transfer
    int bytes_sent = 0;                 // The total bytes successfully sent
    int last_mult = OUTPUTTED;          // The last displayed percentage multiple
    int curr_mult;                      // The current percent multiple being returned

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
                perror("File read error: ");
                status = FAIL;
                break;
            }
            // Create a data packet and send it to the server.
            else
            {
                // Create a data packet and send it to the server using Stop-and-Wait.
                if ((packet = create_data_message(next_seq, bytes_read, buffer)))
                {
                    // If the packet is acknowledged.
                    if (stop_and_wait_send(sockfd, dest, packet, DATA_MSG, timeout, verbose))
                    {
                        // Output the percentage.
                        bytes_sent += bytes_read;
                        curr_mult = output_sent_progress(bytes_sent, filesize, last_mult);
                        if (curr_mult != OUTPUTTED) last_mult = curr_mult;
                        // Update the sequence number.
                        next_seq = (next_seq == 1) ? 0 : 1;
                    }
                    // An error occurred while sending the packet.
                    else
                    {
                        status = FAIL;
                        break;
                    }
                    // Free allocated memory for the data message.
                    free(packet);
                }
            }
        }
        // End the file transfer.
        if (status != FAIL)
            status = end_transfer(sockfd, dest, file, filename,
                                  filesize, next_seq, timeout, verbose);
    }
    // Return the status of the file transfer.
    return status;
}

/*
 * Outputs the percentage progress of the file transfer.
 */
int output_sent_progress (int bytes_sent, int total_bytes, int last_mult)
{
    int percentage = 100 * ((double) bytes_sent / (double) total_bytes);
    int multiple = (percentage / OUTPUT_PCT);

    // If the percentage has not been printed out before, print the progress.
    if ((percentage % OUTPUT_PCT == 0) && (multiple != last_mult))
    {
        if (total_bytes < kB)
            printf("%d/%d\tB sent ..... %d%% complete\n", bytes_sent, total_bytes, percentage);
        else if (total_bytes >= kB && total_bytes < MB)
            printf("%d/%d\tkB sent ..... %d%% complete\n", B_TO_KB(bytes_sent),
                   B_TO_KB(total_bytes), percentage);
        else
            printf("%d/%d\tMB sent ..... %d%% complete\n", B_TO_MB(bytes_sent),
                   B_TO_MB(total_bytes), percentage);

        // Return the newly outputted multiple.
        return multiple;
    }
    // Percentage has been outputted before.
    return OUTPUTTED;
}

/*
 * Transfers a file to the UDP server, via the Reliable File Transfer Protocol (RFTP).
 */
int rftp_transfer_file (char *server_name, char *port_number, char *filename,
        int timeout, int verbose)
{
    host_t server;                      // Server host
    control_message *init = NULL;       // Initialization message
    int filesize;                       // The size of the file being transferred
    int status = FAIL;                  // Status of the file transfer

    // Create a socket and listen on port number.
    int sockfd = create_client_socket(server_name, port_number, &server);
    printf("Trying to initiate a file transfer with %s:%s ...\n", server_name, port_number);

    // If the transfer was initialized, begin transferring the file.
    if ((init = initialize_transfer(sockfd, &server, filename, timeout, verbose)))
    {
        // Get the filesize of the file transfer.
        printf("File transfer initialized.\n\n");
        filesize = ntohl(init->fsize);

        // Give information on file transfer.
        if (filesize < kB)
            printf("Sending %s (%d B) to %s ...\n", filename, filesize, server_name);
        else if (filesize >= kB && filesize < MB)
            printf("Sending %s (%d kB) to %s ...\n", filename, B_TO_KB(filesize), server_name);
        else
            printf("Sending %s (%d MB) to %s ...\n", filename, B_TO_MB(filesize), server_name);

        // Transfer the file to the server.
        status = transfer_file(sockfd, &server, filename, filesize, timeout, verbose);
    }
    // Return the status of the file transfer.
    free(init);
    return status;
}
