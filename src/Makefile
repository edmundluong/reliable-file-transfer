all: rftp rftpd

# RFTP
rftp: rftp.o rftp-client.o rftp-messages.o udp-sockets.o udp-client.o
	gcc -g rftp.o rftp-client.o rftp-messages.o udp-sockets.o udp-client.o -o rftp
rftp.o: rftp.c rftp-client.o
	gcc -Wall -g -c rftp.c

# RFTPD
rftpd: rftpd.o rftp-server.o rftp-messages.o udp-sockets.o udp-server.o
	gcc -g rftpd.o rftp-server.o rftp-messages.o udp-sockets.o udp-server.o -o rftpd
rftpd.o: rftpd.c rftp-server.o
	gcc -Wall -g -c rftpd.c

#RFTP Client
rftp-client.o: rftp-client.h rftp-messages.o udp-sockets.o udp-client.o
	gcc -Wall -g -c rftp-client.c

#RFTP Server
rftp-server.o: rftp-server.h rftp-messages.o udp-sockets.o udp-server.o
	gcc -Wall -g -c rftp-server.c

#RFTP Messages
rftp-messages.o: rftp-messages.h
	gcc -Wall -g -c rftp-messages.c

#UDP Sockets
udp-sockets.o: udp-sockets.h
	gcc -Wall -g -c udp-sockets.c

#UDP Client
udp-client.o: udp-client.h udp-sockets.o
	gcc -Wall -g -c udp-client.c

#UDP Server
udp-server.o: udp-server.h udp-sockets.o
	gcc -Wall -g -c udp-server.c

clean:
	rm -f *.o rftp rftpd
