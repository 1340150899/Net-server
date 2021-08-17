#ifndef _EVENTLOOP_THREAD_H_
#define _EVENTLOOP_THREAD_H_

#include <iostream>
#include <string>
#include <thread> 
#include "EventLoop.h"
using namespace std;

class Eventloopthread {
	private:
		thread th_;
		thread::id threadid_;
		string threadname_;
		EventLoop* loop_;
		
	public:
	
		Eventloopthread();
		~Eventloopthread();

		EventLoop* Getloop();
		
		void Start();
		
		void Threadfunc();
};

#endif
