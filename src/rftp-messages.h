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

#define FILENAME_MSS 1460       // Filename maximum segment.
#define DATA_MSS 1464           // Data maximum segment.

#include <stdint.h>

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
 * Function prototypes.
 */
control_message *create_init_message();
control_message *create_exit_message();
data_message *create_data_message();

#endif /* RFTP_MESSAGES_H_ */
