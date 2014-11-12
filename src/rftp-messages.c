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

#include <stdlib.h>
#include <arpa/inet.h>
#include <poll.h>
#include "rftp-messages.h"

/*
 * Creates an initiation control message to signal the start of a file transfer session.
 */
control_message *create_init_message ()
{
    control_message *msg = malloc(sizeof(*msg));
    if (msg)
    {
        msg->type    = 1;       // Initialization message is always type 1.
        msg->seq_num = 0;       // Initialization message is always seq 0.
    }

    return msg;
}

/*
 * Creates a termination control message to signal the end of a file transfer session.
 */
control_message *create_exit_message ()
{
    control_message *msg = malloc(sizeof(*msg));
    if (msg)
    {
        msg->type = 2;  // Termination message is always type 2.
    }

    return msg;
}

/*
 * Creates an empty data message.
 */
data_message *create_data_message ()
{
    data_message *msg = malloc(sizeof(*msg));
    if (msg)
    {
        msg->type = 3;  // Data message is always type 3.
    }

    return msg;
}

/*
 * Creates a basic RFTP message.
 */
rftp_message *create_rftp_message ()
{
    return (rftp_message*) malloc(sizeof(rftp_message*));
}

/*
 * Receives a RFTP message from a host.
 */
rftp_message *receive_rftp_message (int sockfd, host_t *source)
{
    // Create a new RFTP message.
    rftp_message *msg = create_rftp_message();

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
rftp_message *receive_rftp_message_without_timeout (int sockfd, int timeout,
                                                    host_t *source)
{
    // Create the message and poll structures.
    rftp_message *msg = create_rftp_message();
    struct pollfd fd = { .fd = sockfd, .events = POLLIN };

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
