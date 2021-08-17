#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include<functional>
#include<string>
#include<map>
#include<mutex>
#include"Socket.h"
#include"Channel.h"
#include"EventLoop.h"
#include"Tcpconnection.h"
#include"EventLoopthreadpool.h"
using namespace std;

#define MAXCONNECTION 20000

class Tcpserver{
	public:
		typedef shared_ptr<Tcpconnection> spTcpconnection;
		typedef function<void (const spTcpconnection&, string&)> Messagecallback;
		
		typedef function<void (const spTcpconnection&)> Callback;
		
		Tcpserver(EventLoop* loop, const int port, const int threadnum = 0);
		~Tcpserver();
		
		void Start();
		
		void Setnewconncallback(const Callback& cb){
			newconnectioncallback_ = cb;
		}
		
		void Setmessagecallback(const Messagecallback& cb){
			messagecallback_ = cb;
		}

		
		void Setsendcompletecallback(const Callback& cb){
			sendcompletecallback_ = cb;
		}
		
		void Setclosecallback(const Callback& cb){
			closecallback_ = cb;
		}
		
		void Seterrorcallback(const Callback& cb){
			errorcallback_ = cb;
		}
	
	private:
		Socket serversocket_;
		
		EventLoop *loop_;
		
		Channel serverchannel_;
		
		int conncount_;
		
		map<int, shared_ptr<Tcpconnection>>	tcpconnlist_;
		
		mutex mutex_;
		
		Eventloopthreadpool eventloopthreadpool;
		
		Callback newconnectioncallback_;
		Messagecallback messagecallback_;
		Callback sendcompletecallback_;
		Callback closecallback_;
		Callback errorcallback_;
		
		void Onnewconnection();
		void Removeconnection(const shared_ptr<Tcpconnection> sptcpconnection);
		void Onconnectionerror();
		
};

#endif
