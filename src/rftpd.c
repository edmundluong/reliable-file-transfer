/*
 *  Name        : rftpd.c
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong
 *  Description : Server for the Reliable File Transfer Protocol (RFTP) daemon.
 *
 *  CS 3357a Assignment 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#define DEFAULT_TIMEWAIT 30     // Default transmission timeout: 50 milliseconds.
#define DEFAULT_PORT 5000       // Default port number: 5000.

int main (int argc, char **argv)
{
    static int verbose_flag = 0;                // Toggles verbose output.
    int timewait            = DEFAULT_TIMEWAIT; // Transmission timeout, in milliseconds.
    int port_number         = DEFAULT_PORT;     // Port number the server is listening on.

    while(1)
    {
        int arg, option_index = 0;
        static struct option long_options[] =
        {
                {"verbose", no_argument, &verbose_flag, 1},
                {"timewait", optional_argument, 0, 't'},
                {"port", optional_argument, 0, 'p'},
                {0, 0, 0, 0}
        };
        arg = getopt_long(argc, argv, "vt:p:", long_options, &option_index);

        if (arg == -1) break;
        switch (arg)
        {
            // Enables verbose output.
            case 'v':
                verbose_flag = 1;
                break;
            // Sets transmission response wait time, in milliseconds.
            case 't':
                timewait = optarg;
                break;
            // Sets port number to send messages to.
            case 'p':
                port_number = optarg;
                break;
            // Failure.
            case '?':
                exit(EXIT_FAILURE);
        }
    }
}
