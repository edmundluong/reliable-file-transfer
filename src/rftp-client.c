/*
 *  Name        : rftp-client.c
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong
 *  Date        : November 11, 2014
 *  Description : Implementation of a Reliable File Transfer Protocol (RFTP) client,
 *                used to transfer a file to a RFTP server.
 *
 *  CS 3357a Assignment 2
 */

#include "rftp-protocol.h"
#include "rftp-client.h"
#include "rftp-config.h"
#include "udp-sockets.h"
#include "udp-client.h"
#include "file.h"

#include <stdlib.h>
#include <unistd.h>

/*
 * Attempts to initialize a file transfer session with a RFTP server using
 * the Stop-and-Wait protocol.
 * When the server does not acknowledge an initialization request,
 * another request will be sent when it times out.
 *
 * Return an initiation message for file transfer information if file is valid
 * and a file transfer session has been initiated.
 *
 * Return NULL if there was an error with the file
 * or an error initiating a session with a server.
 */
control_message *request_transfer_session (int sockfd, host_t *dest, char *filename,
        int timeout, int verbose)
{
    rftp_message *init; // A initialization message

    // Construct an initialization message.
    if ((init = create_init_message(filename)))
    {
        // Send initialization message to server via Stop-and-Wait protocol.
        if (stop_and_wait_send(sockfd, dest, init, INIT_MSG, timeout, verbose))
        {
            return (control_message*) init;
        }
    }

    // Transfer failed to initialize.
    free(init);
    return NULL;
}

/*
 * Transfers a file to a RFTP server.
 *
 * Return a successful status if the file transfer was successful.
 * Return a failure status if the file transfer failed.
 */
int transfer_file (int sockfd, host_t *dest, char *filename, int filesize,
        int timeout, int verbose)
{
    FILE *file = NULL;         // The file to be sent
    uint8_t buffer[DATA_MSS];  // Buffer to hold data
    int next_seq = 1;          // The next sequence number
    int bytes_read = 0;        // Number of bytes read from file
    int bytes_sent = 0;        // Total number of bytes successfully sent
    int curr_mult = 0;         // The current percent multiple being returned
    int last_mult = OUTPUTTED; // The last displayed percentage multiple

    // Open the file to be transferred.
    if ((file = get_file(filename, "rb")))
    {
        // While the end of the file has not been reached.
        while (!feof(file))
        {
            // Read data from the file.
            bytes_read = fread(buffer, sizeof(uint8_t), sizeof(buffer), file);
            if (!check_fileread(file)) break;

            // Create a data packet and send it to the server.
            // Continue when data packet was successfully acknowledged.
            if ((send_data_packet(sockfd, dest, next_seq,
                                  bytes_read, buffer, timeout, verbose)))
            {
                // Output the progress of the file transfer.
                bytes_sent += bytes_read;
                curr_mult = output_progress(SEND, bytes_sent, filesize, last_mult);
                if (curr_mult != OUTPUTTED) last_mult = curr_mult;

                // Update the sequence number.
                next_seq = (next_seq == 1) ? 0 : 1;
            }
            // If there was an error sending any data packets.
            else
            {
                fclose(file);
                return FAILURE;
            }
        }
    }

    // Terminate the file transfer session and return the status code.
    fclose(file);
    return end_transfer_session(sockfd, dest, filename,
                                filesize, next_seq, timeout, verbose);
}

/*
 * Attempts to terminate a file transfer session with a UDP server using
 * the Stop-and-Wait protocol.
 * When the server does not acknowledge a termination request,
 * another request will be sent when it times out.
 *
 * Return a successful status code if the termination request was acknowledged.
 * Return a failure status code if the termination request could not be acknowledged.
 */
int end_transfer_session (int sockfd, host_t *dest, char *filename,
        int filesize, int next_seq, int timeout, int verbose)
{
    rftp_message *term; // A termination message

    // Create a termination message.
    if ((term = create_term_message(next_seq, filename, filesize)))
    {
        // Send termination message to server using Stop-and-Wait.
        if (stop_and_wait_send(sockfd, dest, term, TERM_MSG, timeout, verbose))
        {
            return SUCCESS;
        }
    }

    // If an error occurred, return a failure status.
    free(term);
    return FAILURE;
}

/*
 * Transfers a file to the UDP server, via the Reliable File Transfer Protocol (RFTP).
 *
 * Return a successful status code if the file was successfully transferred.
 * Return a failure status code if the file could not be transferred successfully.
 */
int rftp_transfer_file (char *server_name, char *port_number, char *filename,
        int timeout, int verbose)
{
    host_t server;                // Server host
    control_message *init = NULL; // Initialization message
    int filesize = NO_FSIZE;      // The size of the file being transferred
    int status = FAILURE;         // Status of the file transfer

    // Create a socket and listen on port number.
    int sockfd = create_client_socket(server_name, port_number, &server);
    printf("Trying to initiate a file transfer with %s:%s ...\n", server_name, port_number);

    // If the transfer was initialized, begin transferring the file.
    if ((init = request_transfer_session(sockfd, &server, filename, timeout, verbose)))
    {
        // Get the filesize of the file transfer.
        printf("File transfer initialized.\n\n");
        filesize = ntohl(init->fsize);

        // Display file transfer information.
        output_transfer_info(SEND, filename, filesize);

        // Transfer the file to the server.
        status = transfer_file(sockfd, &server, filename, filesize, timeout, verbose);
    }

    // Return the status of the file transfer.
    close(sockfd);
    free(init);
    return status;
}
