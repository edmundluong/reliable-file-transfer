#ifndef UDP_SOCKETS_H
#define UDP_SOCKETS_H

#include <stdint.h>
#include <netdb.h>

#define UDP_MSS 65535   // Maximum amount of data per UDP datagram.

/*
 * message_t struct to hold message contents.
 */
typedef struct
{
    int length;
    uint8_t buffer[UDP_MSS];
} message_t;

/*
 * host_t struct to hold source address for message.
 */
typedef struct
{
    struct sockaddr_in addr;
    socklen_t addr_len;
    char friendly_ip[INET_ADDRSTRLEN];
} host_t;

/*
 * Function prototypes.
 */
struct addrinfo* get_udp_sockaddr (const char* node, const char* port, int flags);
message_t* create_message ();
message_t* receive_message (int sockfd, host_t* source);
message_t* receive_message_without_timeout(int sockfd, int timeout, host_t* source);
int send_message (int sockfd, message_t* msg, host_t* dest);

#endif /* UDP_SOCKETS_H */
