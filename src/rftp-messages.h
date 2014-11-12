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

#ifndef RFTP_MESSAGES_H
#define RFTP_MESSAGES_H

#define FILENAME_MSS 1460       // Filename maximum segment size.
#define DATA_MSS 1464           // Data maximum segment size.
#define RFTP_MSS 1468           // RFTP maximum segment size.

#include <stdint.h>
#include "udp-sockets.h"

/*
 * A control message to initiate and terminate file transfer sessions.
 *
 * RFTP MSS: 1472 bytes.
 */
typedef struct rftp_control_message
{
    uint8_t type;                       // Type of RFTP message: initiation=1 or termination=2.
    uint8_t ack;                        // Acknowledgment identifier: sent=0 or ack=1.
    uint16_t seq_num;                   // Sequence number of the message: 0 or 1.
    uint32_t file_size;                 // Size of the file (in bytes) to be transferred.
    uint32_t filename_length;           // Number of characters in the Filename field.
    uint8_t filename[FILENAME_MSS];     // Name of the file to be transferred.
} control_message;

/*
 * A data message to transfer chunks of file data.
 *
 * RFTP MSS: 1472 bytes.
 */
typedef struct rftp_data_message
{
    uint8_t type;               // Type of RFTP message: data messages always have type=3.
    uint8_t ack;                // Acknowledgment identifier: sent=0 or ack=1.
    uint16_t seq_num;           // Sequence number of the message: 0 or 1.
    uint32_t data_length;       // The number of bytes of data in the Data field.
    uint8_t data[DATA_MSS];     // A sequence of file data bytes.
} data_message;

/*
 * Generic type for RFTP messages.
 *
 * RFTP MSS: 1472 bytes.
 */
typedef struct rftp_base_message
{
    uint8_t type;               // Type of RFTP message: init=1 or exit=2 or data=3.
    uint8_t ack;                // Acknowledgment identifier: sent=0 or ack=1.
    uint16_t seq_num;           // Sequence number of the message: 0 or 1.
    uint32_t length;            // Length of the message.
    uint8_t buffer[DATA_MSS];   // A sequence of buffer bytes.
} rftp_message;

/*
 * Function prototypes.
 */
control_message *create_init_message();
control_message *create_exit_message();
data_message *create_data_message();
rftp_message *create_rftp_message();
rftp_message *receive_rftp_message (int sockfd, host_t *source);
rftp_message *receive_rftp_message_without_timeout(int sockfd, int timeout, host_t *source);
int send_rftp_message (int sockfd, rftp_message *msg, host_t *dest);

#endif /* RFTP_MESSAGES_H_ */
