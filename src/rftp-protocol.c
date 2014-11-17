/*
 * rftp-protocol.c
 *
 *  Created on: Nov 16, 2014
 *      Author: edmund
 */

#include <stdlib.h>
#include <poll.h>

/*
 * Sends a RFTP message to a host.
 *
 * Returns 1 if the message was sent successfully.
 * Returns 0 if the message could not be sent.
 */
int send_rftp_message (int sockfd, host_t *dest, rftp_message *msg)
{
    return sendto(sockfd, msg->buffer, msg->length, 0,
                  (struct sockaddr*) &dest->addr, dest->addr_len);
}

/*
 * Receives a RFTP message from the socket file descriptor.
 *
 * Returns a RFTP message if a message was successfully received from sender.
 *
 * Returns NULL if a message was not received.
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
 * Receives a RFTP message from the specified socket file descriptor,
 * with a specified timeout duration.
 *
 * Returns a RFTP message if a message was successfully received from sender.
 *
 * Returns NULL if a message was not received before request times out.
 */
rftp_message *receive_rftp_message_with_timeout (int sockfd, host_t *source,
        int timeout)
{
    // Prepare to poll the socket for a RFTP message.
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
        // Get the length of the remote IP structure.
        source->addr_len = sizeof(source->addr);

        // Read the message, storing its contents in the message
        // and save the source address.
        msg->length = recvfrom(sockfd, msg->buffer, sizeof(msg->buffer), 0,
                               (struct sockaddr*) &source->addr,
                               &source->addr_len);

        // If the message was successfully received.
        if (msg->length > 0)
        {
            // Convert the source address to a human-readable IP address
            // and store it.
            inet_ntop(source->addr.sin_family, &source->addr.sin_addr,
                      source->friendly_ip, sizeof(source->friendly_ip));

            // Return the message received.
            return msg;
        }
    }

    // Free any allocated memory in case a message was not received.
    if (msg) free(msg);
    return NULL;
}
