/*
 *  Name        : rftp-protocol.c
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong
 *  Date        : November 16, 2014
 *  Description : Implementation of the Reliable File Transfer Protocol (RFTP),
 *                used to for communication and data transmission between RFTP clients/servers.
 *
 *  CS 3357a Assignment 2
 */

#include "rftp-protocol.h"
#include "rftp-messages.h"
#include "rftp-config.h"
#include "data.h"

#include <stdlib.h>
#include <poll.h>
#include <arpa/inet.h>

/*
 * Receives a RFTP message from the socket file descriptor.
 *
 * Return a RFTP message if a message was successfully received from sender.
 * Return NULL if a message was not received.
 */
rftp_message *receive_rftp_message (int sockfd, host_t *source, int verbose)
{
    rftp_message *msg;     // RFTP message.
    control_message *ctrl; // RFTP control message.

    // Create a new RFTP message.
    msg = create_message();

    // Length of the remote IP structure.
    source->addr_len = sizeof(source->addr);

    // Read the message, storing its contents in the message
    // and save the source address.
    msg->length = recvfrom(sockfd, msg->buffer, sizeof(msg->buffer), 0,
                           (struct sockaddr*) &source->addr,
                           &source->addr_len);

    // If a message was successfully received.
    if (msg->length > 0)
    {
        // Convert the source address to a human-readable IP address
        // and store it.
        inet_ntop(source->addr.sin_family, &source->addr.sin_addr,
                  source->friendly_ip, sizeof(source->friendly_ip));

        // Cast the message into a control message type
        // and determine what type of RFTP message was received.
        ctrl = (control_message*) msg;

        // Display verbose message output.
        if (verbose) verbose_msg_output(RECV, ctrl->type, msg);
        ctrl = NULL;

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
 * Return a RFTP message if a message was successfully received from sender.
 * Return NULL if a message was not received before request times out.
 */
rftp_message *receive_rftp_message_with_timeout (int sockfd, host_t *source,
        int timeout, int verbose)
{
    rftp_message *msg;     // RFTP message
    control_message *ctrl; // RFTP control message

    // Prepare to poll the socket for a RFTP message.
    msg = create_message();
    struct pollfd fd = { .fd = sockfd, .events = POLLIN };

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

            // Cast the message into a control message type
            // and determine what type of RFTP message was received.
            ctrl = (control_message*) msg;

            // Display verbose message output.
            if (verbose) verbose_msg_output(RECV, ctrl->type, msg);
            ctrl = NULL;

            // Return the message received.
            return msg;
        }
    }

    // Free any allocated memory in case a message was not received.
    if (msg) free(msg);
    return NULL;
}

/*
 * Sends a RFTP message to a host.
 *
 * Return 1 if the message was sent successfully.
 * Return 0 if the message could not be sent.
 */
int send_rftp_message (int sockfd, host_t *dest, rftp_message *msg,
        int msg_type, int verbose)
{
    int result; // The result of the send operation.

    // Send the message to the specified socket.
    if ((result = sendto(sockfd, msg->buffer, msg->length, 0,
                         (struct sockaddr*) &dest->addr, dest->addr_len)))
    {
        // Display verbose message output.
        if (verbose) verbose_msg_output(SEND, msg_type, msg);
    }

    // Return the result.
    return result;
}

/*
 * Acknowledges a message and sends it to a host.
 *
 * Return a successful status if the acknowledgment was successfully sent.
 * Return a failure status if the acknowledgment could be sent.
 */
int acknowledge_message (int sockfd, host_t *dest, rftp_message *msg,
        int msg_type, int verbose)
{
    control_message *ctrl; // RFTP control message
    data_message *data;    // RFTP data message
    int retval = SEND_ERR; // The status of the send operation

    // Acknowledge a control message.
    if (msg_type == INIT_MSG || msg_type == TERM_MSG)
    {
        ctrl = (control_message*) msg;
        ctrl->ack = ACK;
        retval = send_rftp_message(sockfd, dest, (rftp_message*) ctrl,
                                   msg_type, verbose);
        ctrl = NULL;
    }
    // Acknowledge a data message.
    else
    {
        data = (data_message*) msg;
        data->ack = ACK;
        retval = send_rftp_message(sockfd, dest, (rftp_message*) data,
                                   msg_type, verbose);
        data = NULL;
    }

    // Return the status of the acknowledgment operation.
    return (retval != SEND_ERR);
}

/*
 * Checks the acknowledgment a received RFTP message.
 *
 * Return a successful status if the message was successfully acknowledged.
 * Return a failure status if the message could not be acknowledged.
 */
int check_acknowledgment (rftp_message *orig, rftp_message *response,
        int msg_type)
{
    control_message *control = NULL; // A RFTP control message
    data_message *data = NULL;       // A RFTP data message

    // Handle control message acknowledgments.
    if (msg_type == INIT_MSG || msg_type == TERM_MSG)
    {
        control_message *tmp;

        // Compare original message with response.
        control = (control_message*) response;
        tmp = (control_message*) orig;
        if ((control->type == tmp->type) && (control->ack == ACK)
                && (ntohs(control->seq_num) == ntohs(tmp->seq_num)))
        {
            control = NULL;
            tmp = NULL;
            return SUCCESS;
        }
    }
    // Handle data message acknowledgments.
    else
    {
        data_message *tmp;

        // Compare original message with response.
        data = (data_message*) response;
        tmp = (data_message*) orig;
        if ((data->type == tmp->type) && (data->ack == ACK)
                && (ntohs(data->seq_num) == ntohs(tmp->seq_num)))
        {
            control = NULL;
            tmp = NULL;
            return SUCCESS;
        }
    }

    // If the message could not be acknowledged.
    return FAILURE;
}

/*
 * Outputs the percentage progress of the file transfer.
 *
 * Return a percentage progress multiple, if progress is outputted.
 * Return a outputted status, if the progress has already been outputted.
 */
int output_progress (int trans_type, int bytes_sent, int total_bytes,
        int last_mult)
{
    char *trans_t = NULL; // The transmission type
    int percentage = 0;   // The percentage progress
    int multiple = 0;     // The progress multiple

    // Determine transmission type.
    trans_t = (trans_type == SEND) ? "sent" : "received";
    // Calculate progress.
    percentage = 100 * ((double) bytes_sent / (double) total_bytes);
    multiple = (percentage / OUTPUT_INTVAL);

    // If the percentage has not been printed out before, print the progress.
    if ((percentage % OUTPUT_INTVAL == 0) && (multiple != last_mult))
    {
        if (is_byte(total_bytes))
        {
            printf("%3d/%3d B %s ..... %2d%% complete\n", bytes_sent,
                   total_bytes, trans_t, percentage);
        }
        if (is_kilobyte(total_bytes))
        {
            printf("%3d/%3d kB %s ..... %2d%% complete\n",
                   bytes_to_kilo(bytes_sent), bytes_to_kilo(total_bytes),
                   trans_t, percentage);
        }
        if (is_megabyte(total_bytes))
        {
            printf("%4d/%4d MB %s ..... %2d%% complete\n",
                   bytes_to_mega(bytes_sent), bytes_to_mega(total_bytes),
                   trans_t, percentage);
        }

        // Return the newly outputted progress multiple.
        return multiple;
    }

    // Progress has been outputted before.
    return OUTPUTTED;
}

/*
 * Creates a data packet and sends it to the UDP server using
 * the Stop-and-Wait protocol.
 * When the server does not acknowledge a data packet,
 * the packet will be resent when it times out.
 *
 * Return a successful status if the packet was acknowledged.
 * Return a failure status if the packet could not be sent and acknowledged.
 */
int send_data_packet (int sockfd, host_t *dest, int seq_num, int data_size,
        uint8_t data[DATA_MSS], int timeout, int verbose)
{
    rftp_message *packet = NULL; // Packet of file data to be sent
    int status = FAILURE;        // Status of the packet transfer

    // Create a data packet and send it to the server using the
    // Stop-and-Wait protocol.
    if ((packet = create_data_message(seq_num, data_size, data)))
    {
        // If the packet is acknowledged,
        if (stop_and_wait_send(sockfd, dest, packet, DATA_MSG, timeout,
                               verbose))
        {
            status = SUCCESS;
        }

        // Free the allocated memory for the data message.
        free(packet);
    }

    // Return the status of the packet transfer.
    return status;
}

/*
 * Sends a RFTP message, and waits for an acknowledgment from the server.
 *
 * Returns a successful status if message was sent and acknowledged.
 * Returns a failure status if an error occurred while sending the message.
 */
int stop_and_wait_send (int sockfd, host_t* dest, rftp_message *msg,
        int msg_type, int timeout, int verbose)
{
    rftp_message *response = NULL; // A received RFTP message
    int status = FAILURE;          // The result of the operation
    int retval = SEND_ERR;         // Return value from send operation.

    // Send the message to the server.
    retval = send_rftp_message(sockfd, dest, msg, msg_type, verbose);

    // While the message was successfully sent.
    while (retval != SEND_ERR)
    {
        // Listen for an acknowledgment of the sent packet.
        response = receive_rftp_message_with_timeout(sockfd, dest, timeout,
                                                     verbose);

        // If the message was acknowledged, return a successful status code.
        if (check_acknowledgment(msg, response, msg_type))
        {
            status = SUCCESS;
            break;
        }

        // If the message timed out, send the message again.
        retval = send_rftp_message(sockfd, dest, msg, msg_type, verbose);
    }

    // Free allocated memory and return the result of the operation.
    free(response);
    return status;
}

/*
 * Displays the file transfer information.
 */
void output_transfer_info (int trans_type, char *filename, int filesize)
{
    char *trans_t = NULL; // The transmission type.

    // Determine transmission type.
    trans_t = (trans_type == SEND) ? "Sending" : "Receiving";

    // Display file transfer in bytes.
    if (is_byte(filesize))
    {
        printf("%s %s (%d B) ...\n", trans_t, filename, filesize);
    }
    // Display file transfer in kilobytes.
    if (is_kilobyte(filesize))
    {
        printf("%s %s (%d kB) ...\n", trans_t, filename,
               bytes_to_kilo(filesize));
    }
    // Display file transfer in megabytes.
    if (is_megabyte(filesize))
    {
        printf("%s %s (%d MB) ...\n", trans_t, filename,
               bytes_to_mega(filesize));
    }
}
