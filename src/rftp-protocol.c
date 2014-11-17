/*
 * rftp-protocol.c
 *
 *  Created on: Nov 16, 2014
 *      Author: edmund
 */

#include "rftp-protocol.h"
#include "rftp-config.h"
#include "rftp-messages.h"
#include "data.h"

#include <stdlib.h>
#include <poll.h>
#include <arpa/inet.h>

/*
 * Receives a RFTP message from the socket file descriptor.
 *
 * Returns a RFTP message if a message was successfully received from sender.
 * Returns NULL if a message was not received.
 */
rftp_message *receive_rftp_message (int sockfd, host_t *source, int msg_type,
        int verbose)
{
    // Create a new RFTP message.
    rftp_message *msg = create_message();

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

        // Display verbose message output.
        if (verbose) verbose_msg_output(RECV, msg_type, msg);

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
 * Returns NULL if a message was not received before request times out.
 */
rftp_message *receive_rftp_message_with_timeout (int sockfd, host_t *source,
        int timeout, int msg_type, int verbose)
{
    // Prepare to poll the socket for a RFTP message.
    rftp_message *msg = create_message();
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

            // Display verbose message output.
            if (verbose) verbose_msg_output(RECV, msg_type, msg);

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
 * Returns 1 if the message was sent successfully.
 * Returns 0 if the message could not be sent.
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
        if (verbose) verbose_msg_output(SENT, msg_type, msg);
    }

    // Return the result.
    return result;
}

/*
 * Acknowledges a received RFTP message.
 *
 * Returns 1 if the message was successfully acknowledged.
 * Returns 0 if the message could not be acknowledged.
 */
int acknowledge_message (rftp_message *orig, rftp_message *response,
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
 */
int output_sent_progress (int bytes_sent, int total_bytes, int last_mult)
{
    int percentage = 100 * ((double) bytes_sent / (double) total_bytes);
    int multiple = (percentage / OUTPUT_INTVAL);

    // If the percentage has not been printed out before, print the progress.
    if ((percentage % OUTPUT_INTVAL == 0) && (multiple != last_mult))
    {
        if (is_byte(total_bytes))
        {
            printf("%3d/%3d B sent ..... %2d%% complete\n", bytes_sent,
                   total_bytes, percentage);
        }
        if (is_kilobyte(total_bytes))
        {
            printf("%3d/%3d kB sent ..... %2d%% complete\n",
                   bytes_to_kilo(bytes_sent), bytes_to_kilo(total_bytes),
                   percentage);
        }
        if (is_megabyte(total_bytes))
        {
            printf("%4d/%4d MB sent ..... %2d%% complete\n",
                   bytes_to_mega(bytes_sent), bytes_to_mega(total_bytes),
                   percentage);
        }

        // Return the newly outputted multiple.
        return multiple;
    }

    // Percentage has been outputted before.
    return OUTPUTTED;
}

/*
 * Creates a data packet and sends it to the UDP server using
 * the Stop-and-Wait protocol.
 * When the server does not acknowledge a data packet,
 * the packet will be resent when it times out.
 *
 * Returns a successful status code if the packet was acknowledged.
 * Returns a failure status code if the packet could not be sent and acknowledged.
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
 * Returns a successful status code if message was sent and acknowledged.
 * Returns a failure status code if an error occurred while sending the message.
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
                                                     msg_type, verbose);

        // Send message again when timed out.
        retval = send_rftp_message(sockfd, dest, msg, msg_type, verbose);
    }

    // Free allocated memory and return the result of the operation.
    free(response);
    return status;
}

/*
 * Displays the file transfer information for the sending host.
 */
void show_send_info (char *filesize, char *filename, char *server_name)
{
    // Display file transfer in bytes.
    if (is_byte(filesize))
    {
        printf("Sending %s (%d B) to %s ...\n", filename, filesize,
               server_name);
    }
    // Display file transfer in kilobytes.
    if (is_kilobyte(filesize))
    {
        printf("Sending %s (%d kB) to %s ...\n", filename, B_TO_KB(filesize),
               server_name);
    }
    // Display file transfer in megabytes.
    if (is_megabyte(filesize))
    {
        printf("Sending %s (%d MB) to %s ...\n", filename, B_TO_MB(filesize),
               server_name);
    }
}
