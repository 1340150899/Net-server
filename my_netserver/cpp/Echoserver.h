#ifndef _ECHO_SERVER_H_
#define _ECHO_SERVER_H_

#include<string>
#include"Tcpserver.h"
#include"EventLoop.h"
#include"Tcpconnection.h"
//#include"Timer.h"

class Echoserver {
		
	public:
		typedef shared_ptr<Tcpconnection> spTcpconnection;
		//typedef shared_ptr<Timer> spTimer;
		
		Echoserver(EventLoop* loop, const int port, const int threadnum);
		~Echoserver();
		
		void Start();
	private:
		void Handlenewconnection(const spTcpconnection& sptcpconn);
		void Handlemessage(const spTcpconnection& sptcpconn, string& s);
		void Handlesendcomplete(const spTcpconnection& sptcpnn);
		void Handleclose(const spTcpconnection& stcpconn);
		void Handleerror(const spTcpconnection& sptcpconn);
		
		Tcpserver tcpserver_;
		
};


#endif
