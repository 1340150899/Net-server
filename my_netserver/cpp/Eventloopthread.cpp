//#include <queue>
//#include <mutex>  
//#include <condition_variable>
#include "EventLoopthread.h"
#include <iostream>
#include <sstream>
using namespace std;

Eventloopthread::Eventloopthread()
	:th_(),
	threadid_(-1),
	threadname_("I/O thread: "),
	loop_(nullptr)
{
	
}

Eventloopthread::~Eventloopthread(){
	cout<< "Clean up the EventLoopThread id: "<< this_thread::get_id()<< endl;
	loop_->Quit();
	th_.join();
}

EventLoop* Eventloopthread::Getloop(){
	return loop_;
}

void Eventloopthread::Start(){
	th_ = thread(&Eventloopthread::Threadfunc, this);
}

void Eventloopthread::Threadfunc(){
	EventLoop loop;
	loop_ = &loop;
	
	threadid_ = this_thread::get_id();
	stringstream sin;
	sin << threadid_;
	threadname_ += sin.str();
	
	cout<< "in the thread: "<< threadname_<< endl;
	try{
		loop_->loop();
	}
	catch(bad_alloc& ba){
		cerr << "bad_alloc caught in EventLoopThread::ThreadFunc loop: " << ba.what() << endl;
	}
}
