/*
 *  Name        : rftp-messages.h
 *  Author      : Edmund Luong <edmundvmluong@gmail.com>
 *  Version     : 1.0
 *  Copyright   : MIT 2014 © Edmund Luong
 *  Date        : November 11, 2014
 *  Description : Messages for the Reliable File Transfer Protocol (RFTP).
 *
 *  CS 3357a Assignment 2
 */

#include <stdlib.h>
#include "rftp-messages.h"

/*
 * Creates an initiation control message to signal the start of a file transfer session.
 */
control_message *create_init_message ()
{
    control_message *msg = malloc(sizeof(*msg));
    if (msg)
    {
        msg->type    = 1;       // Initialization message is always type 1.
        msg->seq_num = 0;       // Initialization message is always seq 0.
    }

    return msg;
}

/*
 * Creates a termination control message to signal the end of a file transfer session.
 */
control_message *create_exit_message ()
{
    control_message *msg = malloc(sizeof(*msg));
    if (msg)
    {
        msg->type = 2;  // Termination message is always type 2.
    }

    return msg;
}

/*
 * Creates an empty data message.
 */
data_message *create_data_message ()
{
    data_message *msg = malloc(sizeof(*msg));
    if (msg)
    {
        msg->type = 3;  // Data message is always type 3.
    }

    return msg;
}
