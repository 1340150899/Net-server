#ifndef _TCP_CONNECTION_H_
#define _TCP_CONNECTION_H_

#include <functional>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <memory>
#include "Channel.h"
#include "EventLoop.h"
using namespace std;

class Tcpconnection : public enable_shared_from_this<Tcpconnection> {
		
	public:
		typedef shared_ptr<Tcpconnection> spTcpconnection;
		typedef function<void (const spTcpconnection)> Callback;
		typedef function<void (const spTcpconnection&, string&)> Messagecallback;
		
		Tcpconnection(EventLoop *loop, int fd, const struct sockaddr_in& clientaddr);
		~Tcpconnection();
		
		int fd()const{
			return fd_;
		}
		
		EventLoop* Getloop()const{
			return loop_;
		}
		
		void Addchanneltoloop();
		
		void Send(const string& s);
			
		void Sendinloop();
		
		void Shutdown();
		
		void Shutdowninloop();
		
		void Handleread();
		
		void Handlewrite();
		
		void Handleerror();
		
		void Handleclose();
		
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
		
		void Setconnectioncleanup(const Callback& cb){
			connectioncleanup_ = cb;
		}
		
		void Setasyncprocessing(const bool asyncprocessing){
			asyncprocessing_ = asyncprocessing;
		}
		
	private:
		EventLoop* loop_;
		
		unique_ptr<Channel> spchannel_;
		
		int fd_;

		struct sockaddr_in clientaddr_;
		
		bool halfclose_;
		
		bool disconnected_;
		
		bool asyncprocessing_;
		
		string bufferin_;
		string bufferout_;
		
		Messagecallback messagecallback_;
		Callback sendcompletecallback_;
		Callback closecallback_;
		Callback errorcallback_;
		Callback connectioncleanup_;
		
};

#endif
