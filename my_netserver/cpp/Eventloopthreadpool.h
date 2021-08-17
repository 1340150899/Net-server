#ifndef _EVENTLOOP_THREAD_POOL_H_
#define _EVENTLOOP_THREAD_POOL_H_


//#include <queue>
//#include <mutex>  
//#include <condition_variable>
#include <iostream>
#include <vector>
#include <string>
#include "EventLoop.h"
#include "EventLoopThread.h"
using namespace std;

class Eventloopthreadpool {
	
	private:
		EventLoop* mainloop_;
		int threadnum_;
		vector<Eventloopthread*> threadlist_;
		int index_;
		
	public:
	
	
		Eventloopthreadpool(EventLoop* mainloop, int threadnum = 0);
		~Eventloopthreadpool();

		void Start();
		
		EventLoop* Getnextloop();
};

#endif
