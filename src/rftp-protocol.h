/*
 * rftp-protocol.h
 *
 *  Created on: Nov 16, 2014
 *      Author: edmund
 */

#ifndef RFTP_PROTOCOL_H_
#define RFTP_PROTOCOL_H_

int send_rftp_message (int sockfd, host_t *dest, rftp_message *msg);
rftp_message *receive_rftp_message (int sockfd, host_t *source);
rftp_message *receive_rftp_message_with_timeout(int sockfd, host_t *source, int timeout);
int acknowledge_message (rftp_message *orig, rftp_message *response, int msg_type);

#endif /* RFTP_PROTOCOL_H_ */
