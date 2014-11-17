/*
 * rftp-protocol.h
 *
 *  Created on: Nov 16, 2014
 *      Author: edmund
 */

#ifndef RFTP_PROTOCOL_H_
#define RFTP_PROTOCOL_H_

#include "rftp-messages.h"

#define SEND_ERR -1 // RFTP send error code

/*
 * Function prototypes
 */
rftp_message *receive_rftp_message (int sockfd, host_t *source,
        int msg_type, int verbose);
rftp_message *receive_rftp_message_with_timeout(int sockfd, host_t *source,
        int timeout, int msg_type, int verbose);
int send_rftp_message (int sockfd, host_t *dest, rftp_message *msg,
        int msg_type, int verbose);
int send_data_packet (int sockfd, host_t *dest, int seq_num,
        int data_size, uint8_t data[DATA_MSS], int timeout, int verbose);
int acknowledge_message (rftp_message *orig, rftp_message *response, int msg_type);
int stop_and_wait_send (int sockfd, host_t* dest, rftp_message *msg,
        int msg_type, int timeout, int verbose);
int output_progress (int bytes_sent, int total_bytes, int last_mult);
void show_send_info (char *filename, char *filesize, char *server_name);

#endif /* RFTP_PROTOCOL_H_ */
