#ifndef UDP_SOCKETS_H
#define UDP_SOCKETS_H

#include <stdint.h>
#include <netdb.h>

/*
 * host_t struct to hold source addresses for RFTP message.
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

#endif /* UDP_SOCKETS_H */
