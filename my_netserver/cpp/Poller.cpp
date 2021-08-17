#include"Poller.h"
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<error.h>

#define EVENTMAXNUM 4096

#define TIMEOUT 1000;

Poller::Poller()
	: pollfd_(-1), 
	eventlist_(EVENTMAXNUM), 
	channelmap_(), 
	mutex_()
{
	pollfd_ = epoll_create(256);
	if(pollfd_ == -1){
		perror("epoll_create 1");
		exit(1);
	}
	cout<< "epoll_create: "<< pollfd_<< endl;
}


Poller::~Poller(){
	close(pollfd_);
}

void Poller::poll(ChannelList &activechannellist){
	int timeout = TIMEOUT;
	int nfds = epoll_wait(pollfd_, &*eventlist_.begin(), (int)eventlist_.capacity(), timeout);
	if(nfds == -1){
		perror("epoll wait error");
	}
	
	//cout<< nfds<< endl;
	
	for(int i = 0; i < nfds; ++i){
		int events = eventlist_[i].events;
		Channel* pchannel = (Channel*)eventlist_[i].data.ptr;
		int fd = pchannel->Getfd();
		map<int, Channel*>::const_iterator iter;
		
		{
			lock_guard<mutex> lock(mutex_);
			iter = channelmap_.find(fd);
		}
		
		if(iter != channelmap_.end()){
			pchannel->SetEvents(events);
			activechannellist.emplace_back(pchannel);
		}else{
			cout<< "not find channel!"<< endl;
		}
	}
	if(nfds == (int)eventlist_.capacity()){
		cout<< "resize:"<< nfds<< endl;
		eventlist_.resize(nfds*1.5);
	}
}


void Poller::AddChannel(Channel* pchannel){
	int fd = pchannel->Getfd();
	struct epoll_event ev;
	ev.events = pchannel->Getevents();
	ev.data.ptr = pchannel;
	{
		lock_guard<mutex> lock(mutex_);
		channelmap_[fd] = pchannel;
	}
	if(epoll_ctl(pollfd_,EPOLL_CTL_ADD, fd, &ev) == -1) {
		perror("epoll add error");
		exit(1);
	}
	
}

void Poller::RemoveChannel(Channel* pchannel){
	int fd = pchannel->Getfd();
	struct epoll_event ev;
	ev.events = pchannel->Getevents();
	ev.data.ptr = pchannel;
	{
		lock_guard<mutex> lock(mutex_);
		channelmap_.erase(fd);
		
	}
	
	if(epoll_ctl(pollfd_, EPOLL_CTL_DEL, fd, &ev) == -1){
		perror("epoll del error");
		exit(1);
	}
}

void Poller:: UpdateChannel(Channel* pchannel){
	int fd = pchannel->Getfd();
	struct epoll_event ev;
	ev.events = pchannel->Getevents();
	ev.data.ptr = pchannel;
	
	if(epoll_ctl(pollfd_, EPOLL_CTL_MOD, fd, &ev) == -1){
		perror("epoll update error");
		exit(1);
	}

}


