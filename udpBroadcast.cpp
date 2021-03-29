
// Source for send: https://gist.github.com/jimfinnis/6823802, read 6.9.2020
// Additional information for listener: https://gist.github.com/hostilefork/f7cae3dc33e7416f2dd25a402857b6c6, read 6.9.2020

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <gmp.h>
#include "encryptionFunctions.h"
//#include "udpclient.h"

bool udpSendBroadcast(const char *msg){
    sockaddr_in servaddr;
    int fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd<0){
        perror("cannot open socket");
        return false;
    }

    // The below setsockopt stops multicasts from being received by the sender of said multicast
    u_int yes = 0;
    if (
        setsockopt(
            fd, IPPROTO_IP, IP_MULTICAST_LOOP, (char*) &yes, sizeof(yes)
        ) < 0
    ){
       perror("ip multicast loop disable failed");
       return false;
    }
    
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("224.0.0.132"); // Ŕandomly selected multicast address
    servaddr.sin_port = htons(44803); // Randomly selected non-reserved port according to wikipedia.org/List_of_TCP_and_UDP_port_numbers
    if (sendto(fd, msg, strlen(msg)+1, 0, // +1 to include terminator
               (sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        perror("cannot send message");
        close(fd);
        return false;
    }
    close(fd);
    return true;
}


void udpBroadcastListener(mpz_t n, mpz_t d) {
	sockaddr_in servaddr;
	int fd = socket(AF_INET,SOCK_DGRAM,0);
	if(fd<0){
		perror("cannot open socket");
		return;
	}

	 // allow multiple sockets to use the same PORT number
    //
    u_int yes = 1;
    if (
        setsockopt(
            fd, SOL_SOCKET, SO_REUSEADDR, (char*) &yes, sizeof(yes)
        ) < 0
    ){
       perror("Reusing ADDR failed");
       return;
    }

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // Ŕandomly selected multicast address
    servaddr.sin_port = htons(44803); // Randomly selected non-reserved port according to wikipedia.org/List_of_TCP_and_UDP_port_numbers

    // Bind to socket
    if (bind(fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
    	perror("bind");
    	return;
    }

    // join multicast
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr("224.0.0.132");
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (
        setsockopt(
            fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq)
        ) < 0
    ){
        perror("setsockopt");
        return;
    }

    // now just enter a read-print loop
    //
    while (1) {
        char msgbuf[512];
        socklen_t addrlen = sizeof(servaddr);
        int nbytes = recvfrom(
            fd,
            msgbuf,
            512,
            0,
            (struct sockaddr *) &servaddr,
            &addrlen
        );
        if (nbytes < 0) {
            perror("recvfrom");
            return;
        }
        msgbuf[nbytes] = '\0';
        puts("received broadcast:");
        puts(msgbuf);

        mpz_t msgbufAsMpz;
        mpz_init(msgbufAsMpz);
        //mpz_import(msgbufAsMpz, strlen(msgbuf)/sizeof(msgbuf[0]), 1, sizeof(msgbuf[0]), 0, 0, msgbuf);
        mpz_set_str(msgbufAsMpz, msgbuf, 10);


        decryptCharArray(n, d, msgbufAsMpz);
        mpz_clear(msgbufAsMpz);
    }
}