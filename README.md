Reliable File Transfer Protcol
======================

An implementation of the [Stop-and-Wait protocol](http://en.wikipedia.org/wiki/Stop-and-wait_ARQ) for reliable file transfer and data transmission over UDP sockets.

A file, up to 2GB in size, may be transferred from one location to another using the RFTP client and RFTP server simultaneously.

Usage
======================

Run the Makefile to build the executables.

To run the RFTP server daemon:

    ./rftpd [OPTIONS...] [OUTPUT DIRECTORY]

Example: ./rftpd downloads

This will bind a UDP socket on a port number (port 5000 by default), and listen for any incoming file transfer requests. When a file transfer request is received, the server will receive the file from the client, and save the file in the specified output directory.

There are additional options which can be combined and used for the server daemon:

* <b>-v or --verbose</b> : Enables verbose output for message tracking.
        
        ./rftpd -v downloads
    
* <b>-t or --timewait</b> : The number of seconds to wait before exitting
        
        ./rftpd -t 20 downloads
    
* <b>-p or --port</b> : The port on which the server is listening on.
        
        ./rftpd -p 5001 downloads

To run the RFTP client:

    ./rftp [SERVER NAME] [FILE]
    
Example: ./rftp localhost archive.zip
    
This will attempt to send the specified file to the specified server name (or IP address). The client will use port 5000 by default. The file that can be sent is restricted to a maximum of 2GB.

There are additional options which can be combined and used for the client:

* <b>-v or --verbose</b> : Enables verbose output for message tracking.
        
        ./rftp -v localhost archive.zip

* <b>-t or --timeout</b> : The number of milliseconds to wait before retransmitting a message
        
        ./rftp -t 10 localhost archive.zip

* <b>-p or --port</b> : The port on which the server is listening on.
        
        ./rftpd -p 5001 localhost archive.zip


