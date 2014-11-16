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
    // Create the full pathname.
    char* path = malloc(strlen(output_dir) + strlen(filename) + 2);
    if (path)
    {
        path[0] = 0;
        strcat(path, output_dir);
        strcat(path, "/");
        strcat(path, filename);
    }
    // Create the directory and return the file pointer.
    mkdir(output_dir, 0700);
    FILE* file = fopen(path, "wb");
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
    if (verbose)
        verbose_msg_output(RECV, msg->type, (rftp_message*) msg);
    if (msg)
    {
        // If the message is an initialization message.
        if (msg->type == INIT_MSG && ntohs(msg->seq_num) == 0)
        {
            // Acknowledge the message and send it back to the client.
            msg->ack = ACK;
            int retval = send_rftp_message(sockfd, source, (rftp_message*) msg);
            if (verbose)
                verbose_msg_output(SENT, msg->type, (rftp_message*) msg);
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
    int result = 1; // Result of the termination

    // Close the target file.
    fclose(target);
    // Acknowledge the termination message and send it back to client.
    term->ack = ACK;
    int retval = send_rftp_message(sockfd, source, (rftp_message*) term);
    if (verbose)
        verbose_msg_output(SENT, term->type, (rftp_message*) term);
    // Go into a waiting state for any duplicate termination requests.
    control_message *dupe;
    while ((dupe = (control_message*) receive_rftp_message_with_timeout(
            sockfd, source, time_wait)) && retval != -1)
    {
        if (verbose)
            verbose_msg_output(RECV, dupe->type, (rftp_message*) dupe);
        // Resend termination acknowledgment.
        dupe->ack = ACK;
        retval = send_rftp_message(sockfd, source, (rftp_message*) dupe);
        if (verbose)
            verbose_msg_output(SENT, dupe->type, (rftp_message*) dupe);
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
    control_message *term = NULL;       // A termination message
    data_message *data = NULL;          // A data message
    FILE *target = NULL;                // The target file
    int next_seq = 1;                   // The next expected sequence number
    int status = 1;                     // The status of the file transfer
    int bytes_recv = 0;                 // The total bytes received
    int last_mult = -1;                // The last multiple of percentage output
    int curr_mult;             // The current percentage multiple being returned

    // Create the output directory and output file.
    target = create_dir_and_file(output_dir, filename);
    // Receive data from the client until a termination message is sent.
    rftp_message *msg = receive_rftp_message(sockfd, source);
    while ((term = (control_message*) msg)->type != TERM_MSG)
    {
        data = (data_message*) msg;
        if (verbose)
            verbose_msg_output(RECV, data->type, (rftp_message*) data);
        // If the received data packet is not a duplicate, write the data to file.
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
            if (verbose)
                verbose_msg_output(SENT, data->type, (rftp_message*) data);
            // Give an output of received data.
            bytes_recv += ntohl(data->data_len);
            curr_mult = output_received_progress(bytes_recv, filesize,
                                                 last_mult);
            if (curr_mult != -1)
                last_mult = curr_mult;
            // Update the next expected sequence number.
            next_seq = (next_seq == 1) ? 0 : 1;
        }
        // Receive another message from the client.
        msg = receive_rftp_message(sockfd, source);
        if (verbose)
            verbose_msg_output(RECV, ((data_message*) msg)->type, msg);
    }
    // If a termination message was given, end the file transfer.
    if (term->type == TERM_MSG)
    {
        status = end_receive(sockfd, source, term, target, time_wait, verbose);
        term = NULL;
    }
    // Free allocated memory and return the status of the file transfer.
    if (msg)
        free(msg);
    if (term)
        free(term);
    if (data)
        free(data);
    return status;
}

/*
 * Outputs the percentage of the file transfer.
 */
int output_received_progress (int bytes_recv, int total_bytes, int last_mult)
{
    int percentage = 100 * ((double) bytes_recv / (double) total_bytes);
    int multiple = (percentage / OUTPUT_PCT);

    // If the percentage has not been printed out before, print the progress.
    if ((percentage % OUTPUT_PCT == 0) && (multiple != last_mult))
    {
        if (total_bytes < kB)
            printf("%d/%d\tB received ..... %d%% complete\n", bytes_recv,
                   total_bytes, percentage);
        else if (total_bytes >= kB && total_bytes < MB)
            printf("%d/%d\tkB received ..... %d%% complete\n",
                   B_TO_KB(bytes_recv), B_TO_KB(total_bytes), percentage);
        else
            printf("%d/%d\tMB received ..... %d%% complete\n",
                   B_TO_MB(bytes_recv), B_TO_MB(total_bytes), percentage);
        return multiple;
    }

    return -1;
}

/*
 * Receives a file from the UDP client, via the Reliable File Transfer Protocol (RFTP).
 */
int rftp_receive_file (char *port_number, char *output_dir, int time_wait,
        int verbose)
{
    host_t client;              // Client host
    char *filename = NULL;      // Name of the file being transferred
    int filesize;               // Size of the file being transferred
    int status = 0;             // Status of the file transfer

    // Create a socket and listen on port number.
    int sockfd = create_server_socket(port_number);
    printf("Listening on port %s for a file transfer request ...\n",
           port_number);
    // If a file transfer was initialized.
    control_message *init = initialize_receive(sockfd, &client, verbose);
    if (init)
    {
        // Get the file's information from the init message.
        printf("File transfer initialized.\n\n");
        filesize = ntohl(init->fsize);
        filename = malloc(ntohl(init->fname_len) + 1);
        memcpy(filename, init->fname, ntohl(init->fname_len));
        filename[ntohl(init->fname_len)] = '\0';
        // Display the file transfer information.
        if (filesize < kB)
            printf("Receiving %s (%d B) from %s into %s ...\n", filename,
                   filesize, client.friendly_ip, output_dir);
        else if (filesize >= kB && filesize < MB)
            printf("Receiving %s (%d kB) from %s into %s ...\n", filename,
                   B_TO_KB(filesize), client.friendly_ip, output_dir);
        else
            printf("Receiving %s (%d MB) from %s into %s ...\n", filename,
                   B_TO_MB(filesize), client.friendly_ip, output_dir);
        // Receive the file from the client.
        status = receive_file(sockfd, &client, filename, filesize, output_dir,
                              time_wait, verbose);
        // Report the status of the file transfer.
        if (status)
            printf("\n%s was successfully received from %s into %s.\n",
                   filename, client.friendly_ip, output_dir);
        else
            printf("\nERROR: Could not successfully receive %s from %s.\n",
                   filename, client.friendly_ip);
    }
    // Return status of the file transfer.
    free(filename);
    free(init);
    return status;
}
