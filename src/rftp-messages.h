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

#include "udp-sockets.h"

#include <stdio.h>
#include <stdint.h>

/*
 * Message-oriented macros
 */
#define FNAME_MSS 1460  // Filename maximum segment size
#define DATA_MSS 1464   // Data maximum segment size
#define RFTP_MSS 1472   // RFTP maximum segment size
#define INIT_MSG 1      // File transfer initiation message
#define TERM_MSG 2      // File transfer termination message
#define DATA_MSG 3      // File transfer data message
#define NAK 0           // Unacknowledged message
#define ACK 1           // Acknowledged message
#define SENT 0          // Sent message
#define RECV 1          // Received message
#define DATA_HEADER 8   // Data message header size
#define CTRL_HEADER 12  // Control message header size

/*
 * RFTP Message
 *
 * The generic RFTP message type used for casting.
 * The maximum segment size of all RFTP messages is 1472 bytes.
 */
typedef struct rftp_message
{
    int length;                 // RFTP message length
    uint8_t buffer[RFTP_MSS];   // A sequence of buffer bytes
} rftp_message;

/*
 * RFTP Control message
 *
 * Used to initiate or terminate file transfer sessions.
 */
typedef struct rftp_control_message
{
    int length;               // RFTP message length
    uint8_t type;             // Type 1 RFTP message is for initialization
                              // Type 2 RFTP message is for termination
    uint8_t ack;              // Acknowledgment status
    uint16_t seq_num;         // Sequence number of the message
    uint32_t fsize;           // Size of the file, in bytes
    uint32_t fname_len;       // Length of the filename
    uint8_t fname[FNAME_MSS]; // Filename, maximum of 1460 characters
} control_message;

/*
 * RFTP Data message
 *
 * Used to store and transmit data between hosts.
 */
typedef struct rftp_data_message
{
    int length;             // RFTP message length
    uint8_t type;           // Type 3 RFTP message is reserved for data packets
    uint8_t ack;            // Acknowledgment status
    uint16_t seq_num;       // Sequence number of the message
    uint32_t data_len;      // Number of data bytes in the message
    uint8_t data[DATA_MSS]; // Buffer of binary data bytes, maximum of 1464 bytes
} data_message;


/*
 * Function prototypes
 */
rftp_message *create_message();
rftp_message *create_init_message(char *filename);
rftp_message *create_term_message(int seq_num, char *fname, int fsize);
rftp_message *create_data_message(int seq_num, int bytes_read, uint8_t buffer[DATA_MSS]);
void verbose_msg_output(int trans_type, int msg_type, rftp_message* msg);

#endif /* RFTP_MESSAGES_H */
