#include "Socket.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <cstring>
using namespace std;


Socket::Socket(){
	serverfd_ = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == serverfd_){
		perror("Socket create fail");
		exit(-1);
	}
	
	cout<< "server create socket: "<< serverfd_<< endl;
	
}	

Socket::~Socket(){
	close(serverfd_);
	cout<< "server close..."<< endl;
}

void Socket::Setsocketoption(){
	
}

void Socket::Setreuseaddr(){
	int on = 1;
	setsockopt(serverfd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
}

void Socket::Setnonblocking(){
	int opts = fcntl(serverfd_, F_GETFL);
	if(opts < 0){
		perror("fcntl(serverfd_,GETFL)");
		exit(1);
	}
	if(fcntl(serverfd_, F_SETFL, opts | O_NONBLOCK) < 0){
		perror("fcntl(serverfd_, F_SETFL, opts | O_NONBLOCK)");
		exit(1);
	}
	cout<< "Server setnonblocking..."<< endl;
}

bool Socket::Bindaddress(int serverport){
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(serverport);
	int resval = bind(serverfd_, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if(resval == -1){
		close(serverfd_);
		perror("error bind");
		exit(1);
	}
	cout<< "Server bindaddress..."<< endl;
	return true;
}

bool Socket::Listen(){
	if(listen(serverfd_, 8192) < 0){
		perror("error listen");
		close(serverfd_);
		exit(1);
	}
	cout<< "Server listenning..."<< endl;
	return true;
}

int Socket::Accept(struct sockaddr_in& clientaddr){
	socklen_t lengthofsockaddr = sizeof(clientaddr);
	int clientfd = accept(serverfd_, (struct sockaddr*)&clientaddr, &lengthofsockaddr);
	if(clientfd < 0){
		if(errno == EAGAIN){
			return 0;
		}
	}
	return clientfd;
}

bool Socket::Close(){
	close(serverfd_);
	cout<< "Server close..."<< endl;
	return true;
}
	
