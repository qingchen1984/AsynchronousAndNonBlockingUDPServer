#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <signal.h>
#include <errno.h>

#define MAXLENGTH 255

int sock;

void error(const char * msg){
	perror(msg);
	exit(1);
}

void SIGIOHandler(int signalType){
	struct sockaddr_in clientAddress;
	unsigned int clientAddressLength;
	int recvMessageSize;
	char messageFromClient[MAXLENGTH];
	char messageToSendToClient[] = "got your message client";


	do{
		clientAddressLength = sizeof(clientAddress);

		recvMessageSize = recvfrom( sock, messageFromClient, MAXLENGTH, 0, (struct sockaddr *)&clientAddress, &clientAddressLength);
		if( recvMessageSize < 0){
			// error occurs
			//if the error is because of blocking then errno will get set to EWOULDBLOCK
			if( errno == EWOULDBLOCK){
				//wait
			}
			else{
				// receive failed
			}

		}
		else{
			//success in receiving message from client
			// do what ever you want..send message back to client etc..
		}
	}while( recvMessageSize >= 0);
}

int main(int argc, char * argv[]){
	struct sockaddr_in serverAddress;
	unsigned short serverPort;
	struct sigaction handler;

	if(argc != 2){
		error("Invalid number of args!!");
	}

	serverPort = atoi(argv[1]);

	//create socket
	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if( sock<0 ){
		error("Error in creating socket");
	}

	memset( &serverAddress, 0, sizeof(serverAddress) );
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl( INADDR_ANY );
	serverAddress.sin_port = htons(serverPort);

	//bind socket to the local address
	bind(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress) );

	//set handler for SIGIO
	handler.sa_handler = SIGIOHandler;
	//create mask that mask all signals when the current sigio signal is being handled
	sigfillset( &handler.sa_mask );

	handler.sa_flags = 0;

	sigaction(SIGIO, &handler, 0);

	//ensure that all signals related to the socket will get delivered to this process
	//( because multiple process can have access to the same socket which might lead to ambiguity)
	// the below command ensures that the current process owns the socket
	fcntl( sock, F_SETOWN, getpid() );

	//make the socket non blocking and asynchronous for IO operations
	fcntl( sock, F_SETFL, O_NONBLOCK | FASYNC);

	while(1){
		//do other stuffs
		sleep(1);
	}
}