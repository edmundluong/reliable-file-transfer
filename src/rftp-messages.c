/*
 *  Name        : rftp-messages.c
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong
 *  Date        : November 11, 2014
 *  Description : Implementation of messages for the Reliable File Transfer
 *                Protocol, used for communication and data transmission
 *                between RFTP clients and servers.
 *
 *  CS 3357a Assignment 2
 */

#include "rftp-messages.h"
#include "file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

/*
 * Creates a RFTP message.
 */
rftp_message *create_message ()
{
    return (rftp_message*) malloc(sizeof(rftp_message));
}

/*
 * Creates a RFTP control message to signal the start of a file transfer session.
 *
 * Returns a file transfer session initialization message, if successful.
 * Returns NULL if an error occurred while creating the initialization message.
 */
rftp_message *create_init_message (char *filename)
{
    FILE *file = NULL;    // The file to be transferred
    int fsize = NO_FSIZE; // The size of the file
    int fname_len = 0;    // The length of the filename

    // Create a new RFTP control message.
    control_message *msg = (control_message*) create_message();

    // Open the file to read, if it exists.
    if (msg && (file = get_file(filename, "r")))
    {
        // Get the size of the file and its filename length.
        fsize = get_filesize(file);
        fname_len = strlen(filename);
        fclose(file);

        // Construct the initialization message and convert
        // contents into network order, if necessary.
        msg->length = CTRL_HEADER + fname_len;        // RFTP message length
        msg->type = (uint8_t) INIT_MSG;               // Initialization message
        msg->ack = (uint8_t) NAK;                     // Unacknowledged message
        msg->seq_num = htons((uint16_t) 0);           // Initial sequence number
        msg->fsize = htonl((uint32_t) fsize);         // Size of the file
        msg->fname_len = htonl((uint32_t) fname_len); // Length of the filename
        memcpy(msg->fname, filename, fname_len);      // Filename

        // Return initialization message.
        return (rftp_message*) msg;
    }

    // Free any allocated memory in case of file-related failure.
    if (msg) free(msg);
    return NULL;
}

/*
 * Creates a termination control message to signal the end of a file transfer session.
 *
 * Returns a file transfer termination message, if successful.
 * Returns NULL if an error occurred while creating the termination message.
 */
rftp_message *create_term_message (int seq_num, char *filename, int filesize)
{
    int fname_len = 0; // The length of the filename

    // Create a new RFTP control message.
    control_message *msg = (control_message*) create_message();
    if (msg)
    {
        // Get the length of the filename.
        fname_len = strlen(filename);

        // Construct the termination message and convert
        // contents into network order, if necessary.
        msg->length = CTRL_HEADER + fname_len;        // RFTP message length
        msg->type = (uint8_t) TERM_MSG;               // Termination message
        msg->ack = (uint8_t) NAK;                     // Unacknowledged message
        msg->seq_num = htons((uint16_t) seq_num);     // Sequence number
        msg->fsize = htonl((uint32_t) filesize);      // Size of the file
        msg->fname_len = htonl((uint32_t) fname_len); // Length of the filename
        memcpy(msg->fname, filename, fname_len);      // Filename
    }

    // Return termination message.
    return (rftp_message*) msg;
}

/*
 * Creates a data message to store and transmit data between hosts.
 *
 * Returns a data message storing binary data bytes, if successful.
 * Returns NULL if an error occurred while creating the data message.
 */
rftp_message *create_data_message (int seq_num, int bytes_read,
        uint8_t buffer[DATA_MSS])
{
    // Create a new RFTP data message.
    data_message *msg = (data_message*) create_message();
    if (msg)
    {
        // Construct the data message and convert
        // contents into network order, if necessary.
        msg->length = DATA_HEADER + bytes_read;       // RFTP message length
        msg->type = (uint8_t) DATA_MSG;               // Data message type
        msg->ack = (uint8_t) NAK;                     // Unacknowledged message
        msg->seq_num = htons((uint16_t) seq_num);     // Sequence number
        msg->data_len = htonl((uint32_t) bytes_read); // Number of data bytes
        memcpy(msg->data, buffer, bytes_read);        // Binary data bytes
    }

    // Return data message.
    return (rftp_message*) msg;
}

/*
 * Outputs verbose details about a given RFTP message.
 */
void verbose_msg_output (int trans_type, int msg_type, rftp_message* msg)
{

    int data_size = 0;            // Data size of message
    char *msg_t = NULL;           // Message type
    char *ack = NULL;             // Acknowledgment of message
    control_message *ctrl = NULL; // Control message
    data_message *data = NULL;    // Data message

    // Determine the transmission type.
    char *trans_t = (trans_type == SEND) ? "Sent" : "Received";

    // Control messages.
    if (msg_type == INIT_MSG || msg_type == TERM_MSG)
    {
        // Construct strings and display verbose output.
        msg_t = (msg_type == INIT_MSG) ? "INIT MSG" : "TERM MSG";
        ctrl = (control_message*) msg;
        ack = (ctrl->ack == NAK) ? "NAK" : "ACK";
        printf("%s %s[%d] ..... %s\n", trans_t, msg_t, ntohs(ctrl->seq_num),
               ack);
    }
    // Data messages.
    if (msg_type == DATA_MSG)
    {
        // Construct strings and display verbose output.
        msg_t = "DATA_MSG";
        data = (data_message*) msg;
        ack = (data->ack == NAK) ? "NAK" : "ACK";
        data_size = ntohl(data->data_len);
        printf("%s %s[%d] (%d B) ..... %s\n", trans_t, msg_t,
               ntohs(data->seq_num), data_size, ack);
    }
}
