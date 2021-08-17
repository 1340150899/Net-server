#include "TcpServer.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory>
using namespace std;

void Setnonblocking(int fd);

Tcpserver::Tcpserver(EventLoop* loop, const int port, const int threadnum)
	: serversocket_(),
	loop_(loop),
	serverchannel_(),
	conncount_(0),
	eventloopthreadpool(loop, threadnum)
{
	serversocket_.Setreuseaddr();
	serversocket_.Bindaddress(port);
	serversocket_.Listen();
	serversocket_.Setnonblocking();
	
	serverchannel_.SetFd(serversocket_.fd());
	serverchannel_.SetReadHandle(bind(&Tcpserver::Onnewconnection, this));
	serverchannel_.SetErrorHandle(bind(&Tcpserver::Onconnectionerror, this));
}


Tcpserver::~Tcpserver(){}

void Tcpserver::Start(){
	eventloopthreadpool.Start();
	
	serverchannel_.SetEvents(EPOLLIN | EPOLLET);
	loop_->Addchanneltopoller(&serverchannel_);
}

void Tcpserver::Onnewconnection(){
	struct sockaddr_in clientaddr;
	int clientfd;
	while((clientfd = serversocket_.Accept(clientaddr)) > 0){
		cout<< "New client from IP: "<< inet_ntoa(clientaddr.sin_addr)<< ":"<< ntohs(clientaddr.sin_port)<< endl;
		if(++conncount_ >= MAXCONNECTION){
			close(clientfd);
			continue;
		}
		Setnonblocking(clientfd);
		
		
		EventLoop* loop = eventloopthreadpool.Getnextloop();
		
		shared_ptr<Tcpconnection> sptcpconnection = make_shared<Tcpconnection>(loop, clientfd, clientaddr);
		sptcpconnection->Setmessagecallback(messagecallback_);
		sptcpconnection->Setsendcompletecallback(sendcompletecallback_);
		sptcpconnection->Setclosecallback(closecallback_);
		sptcpconnection->Seterrorcallback(errorcallback_);
		sptcpconnection->Setconnectioncleanup(bind(&Tcpserver::Removeconnection, this, placeholders::_1));
		
		{
			lock_guard<mutex> lock(mutex_);
			tcpconnlist_ [clientfd] = sptcpconnection;
		}
		
		sptcpconnection->Addchanneltoloop();
	}
	
}void Tcpserver::Removeconnection(shared_ptr<Tcpconnection> spttcpconnection){
	lock_guard<mutex> lock(mutex_);
	
	--conncount_;
	
	tcpconnlist_.erase(spttcpconnection->fd());
}

void Tcpserver::Onconnectionerror(){
	cout<< "UNKNOWN EVENT"<< endl;
	serversocket_.Close();
}

void Setnonblocking(int fd){
	int opts = fcntl(fd, F_GETFL);
	if(opts < 0){
		perror("fcntl(fd, F_GETFL)");
		exit(1);
	}
	if(fcntl(fd, F_SETFL, opts | O_NONBLOCK) < 0){
		perror("fcntl(fd, F_SETFL, opts | O_NONBLOCK)");
		exit(1);
	}
}
