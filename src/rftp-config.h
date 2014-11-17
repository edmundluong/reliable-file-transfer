/*
 *  Name        : rftp-config.h
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong
 *  Date        : November 16, 2014
 *  Description : A configuration file for the Reliable File Transfer Protocol.
 *
 *  CS 3357a Assignment 2
 */

#ifndef RFTP_CONFIG_H
#define RFTP_CONFIG_H

#define SUCCESS 1               // Success status code
#define FAILURE 0               // Failure status code
#define VERBOSE 1               // Verbose mode on
#define SILENT 0                // Verbose mode off
#define OUTPUTTED -1            // Outputted progress status code

#define DEFAULT_TIMEOUT 50      // Default transmission timeout, in milliseconds
#define DEFAULT_TIME_WAIT 30    // Default server wait state duration, in milliseconds
#define DEFAULT_PORT "5000"     // Default port number
#define OUTPUT_INTVAL 1         // Output interval, in percentage

#endif /* RFTP_CONFIG_H */
