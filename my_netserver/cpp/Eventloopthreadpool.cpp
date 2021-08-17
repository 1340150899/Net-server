#include"Eventloopthreadpool.h"
using namespace std;

Eventloopthreadpool::Eventloopthreadpool(EventLoop* mainloop, int threadnum)
	:mainloop_(mainloop),
	threadnum_(threadnum),
	threadlist_(),
	index_(0)
{
	for(int i = 0; i < threadnum_; ++i){
		Eventloopthread* peventloopthread = new Eventloopthread();
		threadlist_.push_back(peventloopthread);
	}
}

Eventloopthreadpool::~Eventloopthreadpool(){
	cout<< "Clean up the Eventloopthreadpool"<< endl;
	for(int i = 0; i < threadnum_; ++i){
		delete threadlist_[i];
	}
	threadlist_.clear();
}

void Eventloopthreadpool::Start(){
	if(threadnum_ > 0){
		for(int i = 0; i < threadnum_; ++i){
			threadlist_[i]->Start();
		}
	}else{
		
	}
}

EventLoop* Eventloopthreadpool::Getnextloop(){
	if(threadnum_ > 0){
		EventLoop *loop = threadlist_[index_]->Getloop();
		index_ = (index_ + 1) % threadnum_;
		return loop;
	}else{
		return mainloop_;
	}
}
