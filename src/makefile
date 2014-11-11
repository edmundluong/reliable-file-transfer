all: rftp rftpd
rftp: rftp.o
	gcc -g rftp.o -o rftp
rftpd: rftpd.o
	gcc -g rftpd.o -o rftpd
rftp.o: rftp-messages.h rftp.c
	gcc -Wall -g -c rftp.c
rftpd.o: rftp-messages.h rftpd.c
	gcc -Wall -g -c rftpd.c
clean:
	rm -f *.o rftp rftpd
