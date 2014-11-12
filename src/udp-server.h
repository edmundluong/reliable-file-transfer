#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <netdb.h>

/*
 * Function declarations.
 */
int bind_socket (struct addrinfo* addr_list);
int create_server_socket (char* port);

#endif /* UDP_SERVER_H */
