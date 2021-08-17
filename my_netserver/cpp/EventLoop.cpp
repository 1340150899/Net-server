#include "EventLoop.h"
#include <iostream>
#include <sys/eventfd.h>
#include <unistd.h>
#include <stdlib.h>
using namespace std;

int CreateEventfd(){
	int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if(evtfd < 0){
		cout<< "Failed int eventfd"<< endl;
		exit(1);
	}
	return evtfd;
}

EventLoop::EventLoop() : functorlist_(), channellist_(), 
		activechannellist_(), 
		poller_(),
		quit_(true), 
		tid_(this_thread::get_id()), 
		mutex_(), 
		wakeupfd_(CreateEventfd()), 
		wakeupchannel_()
{
	wakeupchannel_.SetFd(wakeupfd_);
	wakeupchannel_.SetEvents(EPOLLIN | EPOLLET);
	wakeupchannel_.SetReadHandle(bind(&EventLoop::Handleread, this));
	wakeupchannel_.SetErrorHandle(bind(&EventLoop::Handleerror, this));
	Addchanneltopoller(&wakeupchannel_);
}

EventLoop::~EventLoop(){
	close(wakeupfd_);
}

void EventLoop::Wakeup(){
	uint64_t one = 1;
	ssize_t n = write(wakeupfd_, (char*)(&one) , sizeof one);
}

void EventLoop::Handleread(){
	uint64_t one = 1;
	ssize_t n = read(wakeupfd_, &one, sizeof one);
}

void EventLoop::Handleerror(){
	
}

void EventLoop::loop(){
	quit_ = false;
	while(!quit_){
		poller_.poll(activechannellist_);
		for(Channel *pchannel : activechannellist_){
			pchannel->HandleEvent();
		}
		activechannellist_.clear();
		ExecuteTask();
	}
}
		   
