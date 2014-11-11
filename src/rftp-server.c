/*
 * rftp-server.c
 *
 *  Created on: Nov 11, 2014
 *      Author: edmund
 */

/*
 * Receives a file from the UDP client, via the Reliable File Transfer Protocol (RFTP).
 */
int rftp_receive_file(char *port_number, char *output_dir, int time_wait, int verbose)
{
    int result = 1;

    // Return status of file transfer.
    if (result)
        return 1;
    else
        return 0;
}
