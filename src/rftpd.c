/*
 *  Name        : rftpd.c
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong
 *  Date        : November 11, 2014
 *  Description : Executable server daemon for the Reliable File Transfer
 *                Protocol used to receive a file from a RFTP client.
 *
 *  CS 3357a Assignment 2
 */

#include "rftp-server.h"
#include "rftp-config.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

// Main program.
int main (int argc, char **argv)
{
    static int verbose_flag = SILENT;  // Toggles verbose output
    int time_wait = DEFAULT_TIME_WAIT; // Transmission timeout in milliseconds
    char *port_number = DEFAULT_PORT;  // Port number to listen on
    char *output_dir = NULL;           // The transfer output directory

    // Handle command line options.
    int arg, option_index = 0;
    static struct option long_options[] =
    {
            {"verbose", no_argument, &verbose_flag, 1},
            {"timewait", optional_argument, 0, 't'},
            {"port", optional_argument, 0, 'p'},
            {0, 0, 0, 0}
    };
    while ((arg = getopt_long(argc, argv, "vt:p:", long_options,
                              &option_index)) != EOF)
    {
        switch (arg)
        {
            case 'v': // Enables verbose output
                verbose_flag = 1;
                break;
            case 't': // Sets transmission response wait time, in milliseconds
                time_wait = atoi(optarg);
                break;
            case 'p': // Sets port number to send messages to
                port_number = optarg;
                break;
            case '?': // Failure
                exit(EXIT_FAILURE);
        }
    }

    // Handle non-option arguments.
    if (optind < argc)
    {
        // Get the output directory from arguments.
        if (!output_dir) output_dir = argv[optind];
    }

    // If an output directory was not supplied, exit the program.
    if (!output_dir)
    {
        printf("ERROR:\n");
        printf("- A output directory must be supplied.\n");
        printf("Sample usage: %s [OPTIONS...] [OUTPUTDIR]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Receive a file transfer from the client and exit the program.
    if (rftp_receive_file(port_number, output_dir, time_wait, verbose_flag))
    {
        exit(EXIT_SUCCESS);
    }
    else
    {
        exit(EXIT_FAILURE);
    }
}
