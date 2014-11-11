/*
 *  Name        : rftp.c
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong
 *  Description : Client for the Reliable File Transfer Protocol (RFTP).
 *
 *  CS 3357a Assignment 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "rftp-messages.h"
#include "rftp-client.h"
#include "udp_sockets.h"
#include "udp_client.h"

#define DEFAULT_TIMEOUT 50      // Default transmission timeout: 50 milliseconds.
#define DEFAULT_PORT "5000"     // Default port number: 5000.

// Main program.
int main (int argc, char **argv)
{
    static int verbose_flag = 0;               // Toggles verbose output.
    int timeout             = DEFAULT_TIMEOUT; // Transmission timeout, in milliseconds.
    char *port_number       = DEFAULT_PORT;    // Port number the server is listening on.
    char *server            = NULL;            // The server name that is receiving the file.
    char *filename          = NULL;            // The name of the file that is being transferred.

    // Handle command line options.
    int arg, option_index = 0;
    static struct option long_options[] =
    {
            {"verbose", no_argument, &verbose_flag, 1},
            {"timeout", optional_argument, 0, 't'},
            {"port", optional_argument, 0, 'p'},
            {0, 0, 0, 0}
    };
    while((arg = getopt_long(argc, argv, "vt:p:", long_options, &option_index)) != EOF)
    {
        switch (arg)
        {
            case 'v':   // Enables verbose output.
                verbose_flag = 1;
                break;
            case 't':   // Sets transmission timeout, in milliseconds.
                timeout = atoi(optarg);
                break;
            case 'p':   // Sets port number to send messages to.
                port_number = optarg;
                break;
            case '?':   // Failure.
                exit(EXIT_FAILURE);
        }
    }
    // Handle non-option arguments.
    for (; optind < argc; ++optind)
    {
        // Get the server name from arguments.
        if (!server) server = argv[optind];
        // Get the filename from arguments.
        else if (!filename) filename = argv[optind];
        // If both are supplied, stop looping.
        else break;
    }
    // If either the server or the filename was not supplied, exit the program.
    if (!server || !filename)
    {
        printf("ERROR:\n");
        if (!server) printf("- A server name must be supplied.\n");
        if (!filename) printf("- A filename must be supplied.\n");
        printf("Sample usage: %s [OPTIONS...] [SERVER] [FILENAME]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Transfer the file to the server and exit the program.
    if (rftp_file_transfer(server, port_number, filename, timeout, verbose_flag))
    {
        printf("%s was successfully transferred.\n", filename);
        exit(EXIT_SUCCESS);
    }
    else
    {
        printf("ERROR: %s failed to transfer.\n", filename);
        exit(EXIT_FAILURE);
    }
}

