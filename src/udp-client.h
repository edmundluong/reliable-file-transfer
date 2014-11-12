#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include "udp-sockets.h"

/*
 * Function prototypes.
 */
int create_client_socket (char* hostname, char* port, host_t* server);

#endif /* UDP_CLIENT_H */
