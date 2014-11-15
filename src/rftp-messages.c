/*
 *  Name        : rftp-messages.h
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong
 *  Date        : November 11, 2014
 *  Description : Messages for the Reliable File Transfer Protocol (RFTP).
 *
 *  CS 3357a Assignment 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <poll.h>
#include "rftp-messages.h"

#define MAX_FILE_SIZE 4000000000 // Maximum transferrable file size is 4GB.

/*
 * Helper method to determine a file's size.
 */
int fsize (FILE *file)
{
    // Seek to EOF, get the pointer position, and seek back to beginning.
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Return size of file.
    return size;
}

/*
 * Creates an initiation control message to signal the start of a file transfer session.
 */
rftp_message *create_init_message (char *filename)
{
    control_message *msg = (control_message*) create_message();
    FILE *file;

    // If the file exists, construct the rest of the initialization message.
    if (msg && (file = fopen(filename, "r")))
    {
        // If the size of the file is valid and can be sent,
        // create an init message.
        int size = fsize(file);
        int fname_len = strlen(filename);
        fclose(file);
        if (size > MAX_FILE_SIZE)
            printf("ERROR: Cannot send files larger than 4GB.\n");
        else
        {
            // Initialization message is always type 1.
            msg->type = (uint8_t) INIT_MSG;
            // Initialization message is always seq 0.
            msg->seq_num = htons((uint16_t) 0);
            // Message is unacknowledged on creation.
            msg->ack = (uint8_t) NAK;
            // The size of file being transferred.
            msg->fsize = htonl((uint32_t) size);
            // The length of the filename.
            msg->fname_len = htonl((uint32_t) fname_len);
            // The name of the file being transferred.
            memcpy(msg->fname, filename, fname_len);
            // Length of the message is 16 bytes of headers and variable filename size.
            msg->length = 16 + (fname_len/8);

            // Return initialization message.
            return (rftp_message*) msg;
        }
    }

    // If an error occurred, return NULL.
    free(msg);
    return NULL;
}

/*
 * Creates a termination control message to signal the end of a file transfer session.
 */
rftp_message *create_term_message (int seq_num, int fsize, char *fname)
{
    control_message *msg = (control_message*) create_message();
    if (msg)
    {
        // Termination message is always type 2.
        msg->type = (uint8_t) TERM_MSG;
        // Sequence number of the message.
        msg->seq_num = htons((uint16_t) seq_num);
        // Message is unacknowledged on creation.
        msg->ack = (uint8_t) NAK;
        // The size of the file being transferred.
        msg->fsize = htonl((uint32_t) fsize);
        // The length of the filename.
        int fname_len = strlen(fname);
        msg->fname_len = htonl((uint32_t) fname_len);
        // The name of the file being transferred.
        memcpy(msg->fname, fname, fname_len);
        // Length of the message is 16 bytes of headers and variable filename size.
        msg->length = 16 + (fname_len/8);
    }

    // Return termination message.
    return (rftp_message*) msg;
}

/*
 * Creates an empty data message.
 */
rftp_message *create_data_message ()
{
    data_message *msg = (data_message*) create_message();
    if (msg)
    {
        msg->type = 3;  // Data message is always type 3.
        msg->ack = 0;  // Message is unacknowledged on creation.
    }

    return (rftp_message*) msg;
}

/*
 * Creates a basic RFTP message.
 */
rftp_message *create_message ()
{
    return (rftp_message*) malloc(sizeof(rftp_message));
}

/*
 * Receives a RFTP message from a host.
 */
rftp_message *receive_rftp_message (int sockfd, host_t *source)
{
    // Create a new RFTP message.
    rftp_message *msg = create_message();

    // Length of the remote IP structure.
    source->addr_len = sizeof(source->addr);

    // Read message, storing its contents in msg->buffer, and the source address in source->addr.
    msg->length = recvfrom(sockfd, msg->buffer, sizeof(msg->buffer), 0,
                           (struct sockaddr*) &source->addr, &source->addr_len);

    // If a message was successfully received.
    if (msg->length > 0)
    {
        // Convert the source address to a human-readable form,
        // storing it in source->friendly_ip.
        inet_ntop(source->addr.sin_family, &source->addr.sin_addr,
                  source->friendly_ip, sizeof(source->friendly_ip));

        // Return the message received.
        return msg;
    }
    // If a message was not received, free the allocated memory and return NULL.
    else
    {
        free(msg);
        return NULL;
    }
}

/*
 * Receives a RFTP message from a host without timeout.
 */
rftp_message *receive_rftp_message_with_timeout (int sockfd, int timeout,
        host_t *source)
{
    // Create the message and poll structures.
    rftp_message *msg = create_message();
    struct pollfd fd = {
            .fd = sockfd,
            .events = POLLIN
    };

    // Poll the socket for specified time.
    int retval = poll(&fd, 1, timeout);

    // If socket receives data to read.
    if (retval == 1 && fd.revents == POLLIN)
    {
        // Length of the remote IP structure.
        source->addr_len = sizeof(source->addr);

        // Read message, storing its contents in msg->buffer and the source address in source->addr.
        msg->length = recvfrom(sockfd, msg->buffer, sizeof(msg->buffer), 0,
                               (struct sockaddr*) &source->addr,
                               &source->addr_len);

        // If a message was successfully received.
        if (msg->length > 0)
        {
            // Convert the source address to a human-readable form,
            // storing it in source->friendly_ip.
            inet_ntop(source->addr.sin_family, &source->addr.sin_addr,
                      source->friendly_ip, sizeof(source->friendly_ip));

            // Return the message received.
            return msg;
        }
    }
    // If a message was not received, free the allocated memory and return NULL.
    free(msg);
    return NULL;
}

/*
 * Sends a RFTP message to a host.
 */
int send_rftp_message (int sockfd, rftp_message *msg, host_t *dest)
{
    return sendto(sockfd, msg->buffer, msg->length, 0,
                  (struct sockaddr*) &dest->addr, dest->addr_len);
}
