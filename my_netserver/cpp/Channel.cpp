#include"Channel.h"
#include<iostream>
#include<sys/epoll.h>
using namespace std;
Channel::Channel():fd_(-1){}

Channel::~Channel(){}

void Channel::HandleEvent(){
	if(events_ & EPOLLRDHUP){
		cout<< "Event EPOLLRDHUP"<< endl;
		closehandler_();
	}
	else if(events_ & (EPOLLIN | EPOLLPRI)){
		readhandler_();
	}
	else if(events_ & EPOLLOUT){
		cout<< "Event EPOLLOUT"<< endl;
		writehandler_();
	}
	else{
		cout<< "Event ERROR"<< endl;
		errorhandler_();
	}
}
