CC=gcc
CFLAGS=-Wall -g -c
LFLAGS=-Wall -g

all: rftp rftpd
clean:
	rm -f *.o rftp rftpd

# RFTP
rftp: rftp.o rftp-client.o rftp-protocol.o rftp-messages.o udp-sockets.o udp-client.o file.o data.o
	$(CC) $(LFLAGS) -o $@ $^
rftp.o: rftp.c rftp-client.h rftp-config.h
	gcc -Wall -g -c rftp.c

# RFTPD
rftpd: rftpd.o rftp-server.o rftp-protocol.o rftp-messages.o udp-sockets.o udp-server.o file.o data.o
	$(CC) $(LFLAGS) -o $@ $^
rftpd.o: rftpd.c rftp-server.h rftp-config.h
	$(CC) $(CFLAGS) -o $@ $<

# RFTP Client
rftp-client.o: rftp-client.c rftp-client.h rftp-config.h rftp-protocol.h udp-sockets.h udp-client.h file.h
	$(CC) $(CFLAGS) -o $@ $<

# RFTP Server
rftp-server.o: rftp-server.c rftp-server.h rftp-config.h rftp-protocol.h udp-sockets.h udp-server.h file.h
	$(CC) $(CFLAGS) -o $@ $<

# RFTP Protocol
rftp-protocol.o: rftp-protocol.c rftp-protocol.h rftp-config.h rftp-messages.h data.h
	$(CC) $(CFLAGS) -o $@ $<

# RFTP Messages
rftp-messages.o: rftp-messages.c rftp-messages.h file.h
	$(CC) $(CFLAGS) -o $@ $<

# UDP Sockets
udp-sockets.o: udp-sockets.c udp-sockets.h
	$(CC) $(CFLAGS) -o $@ $<

# UDP Client
udp-client.o: udp-client.c udp-client.h udp-sockets.h
	$(CC) $(CFLAGS) -o $@ $<

# UDP Server
udp-server.o: udp-server.c udp-server.h udp-sockets.h
	$(CC) $(CFLAGS) -o $@ $<

# Data
data.o: data.c data.h
	$(CC) $(CFLAGS) -o $@ $<

# File
file.o: file.c file.h
	$(CC) $(CFLAGS) -o $@ $<