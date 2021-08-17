#include "Tcpconnection.h"
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

#define BUFSIZE 4096

int recvn(int fd, string& bufferin);
int sendn(int fd, string& bufferout);

Tcpconnection::Tcpconnection(EventLoop* loop, int fd, const struct sockaddr_in& clientaddr)
	: loop_(loop),
	spchannel_(new Channel()),
	
	fd_(fd),
	clientaddr_(clientaddr),
	halfclose_(false),
	disconnected_(false),
	asyncprocessing_(false),
	bufferin_(),
	bufferout_()
{
	spchannel_->SetFd(fd_);
	spchannel_->SetEvents(EPOLLIN | EPOLLET);
	spchannel_->SetReadHandle(bind(&Tcpconnection::Handleread, this));
	spchannel_->SetWriteHandle(bind(&Tcpconnection::Handlewrite, this));
	spchannel_->SetCloseHandle(bind(&Tcpconnection::Handleclose, this));
	spchannel_->SetErrorHandle(bind(&Tcpconnection::Handleerror, this));
}

Tcpconnection::~Tcpconnection(){
	loop_->Removechanneltopoller(spchannel_.get());
	close(fd_);
}

void Tcpconnection::Addchanneltoloop(){
	loop_->Addtask(bind(&EventLoop::Addchanneltopoller, loop_, spchannel_.get()));
}

void Tcpconnection::Send(const string& s){
	bufferout_ += s;
	if(loop_->GetThreadId() == this_thread::get_id()) {
		Sendinloop();
	}else{
		asyncprocessing_ = false;
		
		loop_->Addtask((bind(&Tcpconnection::Sendinloop, shared_from_this())));
	}
}	

void Tcpconnection::Sendinloop(){
	if(disconnected_){
		return;
	}
	
	int result = sendn(fd_, bufferout_);
						   
	if(result > 0){
		uint32_t events = spchannel_->Getevents();
		
		if(bufferout_.size() > 0){
			spchannel_->SetEvents(events | EPOLLOUT);
			
			loop_->Updatechanneltopoller(spchannel_.get());
		}else{
			spchannel_->SetEvents(events & (~EPOLLOUT));
			sendcompletecallback_ (shared_from_this());
			if(halfclose_)
				Handleclose();
		}	
	}else if(result < 0){
		Handleerror();
	}else{
		Handleclose();
	}
}

void Tcpconnection::Shutdown(){
	if(loop_->GetThreadId() == this_thread::get_id()){
		Shutdowninloop();
	}else{
		loop_->Addtask(bind(&Tcpconnection::Shutdowninloop, shared_from_this()));
	}
}

void Tcpconnection::Shutdowninloop(){
	if(disconnected_)
		return;
	cout<< "shutdown"<< endl;
	closecallback_(shared_from_this());
	loop_->Addtask(bind(connectioncleanup_, shared_from_this()));
	disconnected_ = true;
}

void Tcpconnection::Handleread(){
	int result = recvn(fd_, bufferin_);
	
	if(result > 0){
		messagecallback_(shared_from_this(), bufferin_);
	}else if(result == 0){
		Handleclose();
	}else{
		Handleerror();
	}
}

void Tcpconnection::Handlewrite(){
	int result = sendn(fd_, bufferout_);
	if(result > 0){
		uint32_t events = spchannel_->Getevents();
		if(bufferout_.size() > 0){
			spchannel_->SetEvents(events | EPOLLOUT);
			loop_->Updatechanneltopoller(spchannel_.get());
		}else{
			spchannel_->SetEvents(events & (~EPOLLOUT));
			sendcompletecallback_(shared_from_this());
			if(halfclose_)
				Handleclose();
		}
	}else if(result < 0){
		Handleerror();
	}else{
		Handleclose();
	}
}

void Tcpconnection::Handleerror(){
	if(disconnected_)
		return;
	errorcallback_(shared_from_this());
	loop_->Addtask(bind(connectioncleanup_, shared_from_this()));
	disconnected_ = true;
}

void Tcpconnection::Handleclose(){
	if(disconnected_)
		return;
	if(bufferout_.size() > 0 || bufferin_.size() > 0 || asyncprocessing_){
		halfclose_ = true;
		if(bufferin_.size() > 0){
			messagecallback_(shared_from_this(), bufferin_);
		}
	}else{
		loop_->Addtask(bind(connectioncleanup_, shared_from_this()));
		closecallback_(shared_from_this());
		disconnected_ = true;
	}
}

int recvn(int fd, string& bufferin){
	int nbyte = 0;
	int readsum = 0;
	char buffer[BUFSIZE];
	while(true){
		nbyte = read(fd, buffer, BUFSIZE);
		if(nbyte > 0){
			bufferin.append(buffer, nbyte);
			readsum += nbyte;
			if(nbyte < BUFSIZE)
				return readsum;
			else 
				continue;
		}else if(nbyte < 0){
			if(errno == EAGAIN){
				return readsum;
			}
			else if(errno == EINTR){
				cout<< "errno == EINTR"<< endl;
				continue;
			}else{
				perror("recv error");
				return -1;
			}
			
		}else{
			return 0;
		}
	}
}

int sendn(int fd, string& bufferout){
	ssize_t nbyte = 0;
	int sendsum = 0;
	size_t length = 0;
	
	length = bufferout.size();
	if(length >= BUFSIZE){
		length = BUFSIZE;
	}
	while(true){
		nbyte = write(fd, bufferout.c_str(), length);
		if(nbyte > 0){
			sendsum += nbyte;
			bufferout.erase(0, nbyte);
			
			length = bufferout.size();
			if(length >= BUFSIZE){
				length = BUFSIZE;
			}
			if(length == 0){
				return sendsum;
			}
		}else if(nbyte < 0){
			if(errno == EAGAIN){
				cout<< "write errno == EAGAIN not finish!"<< endl;
				return sendsum;
			}else if(errno == EINTR){
				cout<< "write errno == EINTR"<< endl;
				continue;
			}else if(errno == EPIPE){
				perror("write error");
				cout<< "write errno == client send RST"<< endl;
				return -1;
			}else{
				perror("write error");
				cout<< "write error unknow error"<< endl;
				return -1;
			}
		}else{
			return 0;
		}
	}
}
