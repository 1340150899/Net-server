#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_

#include<iostream>
#include<functional>
#include<vector>
#include<thread>
#include<mutex>
#include"Channel.h"
#include"Poller.h"
using namespace std;

class EventLoop {
		
	public:
	
	typedef function<void()> Functor;
	
	typedef vector<Channel*> Channellist;
	
		EventLoop();
		~EventLoop();
	
	void loop();
	
	void Addchanneltopoller(Channel* pchannel){
		poller_.AddChannel(pchannel);
	}
	
	void Removechanneltopoller(Channel *pchannel){
        poller_.RemoveChannel(pchannel);
	}
	
	void Updatechanneltopoller(Channel *pchannel){
        poller_.UpdateChannel(pchannel);
    }		
    
    void Quit()
	{
	        quit_ = true;
	}
	
	thread::id GetThreadId()const{
		return tid_;
	}
	
	void Wakeup();
	
	void Handleread();
	
	void Handleerror();

	void Addtask(Functor functor){
		{
			lock_guard<mutex> lock(mutex_);
			functorlist_.push_back(functor);
		}
		Wakeup();
	}
	
	void ExecuteTask(){
		vector<Functor> functorlist;
		{
			lock_guard<mutex> lock(mutex_);
			functorlist.swap(functorlist_);
		}
		for(Functor & functor : functorlist){
			functor();
		}
		functorlist.clear();
	}
private:
	vector<Functor> functorlist_;
	
	Channellist channellist_;
	
	Channellist activechannellist_;
	
	Poller poller_;
	
	bool quit_;
	
	thread::id tid_;
	
	mutex mutex_;
	
	int wakeupfd_;
	
	Channel wakeupchannel_;
};

#endif
