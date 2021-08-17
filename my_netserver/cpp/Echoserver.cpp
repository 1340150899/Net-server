#include"Echoserver.h"
#include<iostream>
#include<functional>
using namespace std;

Echoserver::Echoserver(EventLoop* loop, const int port, const int threadnum)
	: tcpserver_(loop, port, threadnum)
{
	tcpserver_.Setnewconncallback(bind(&Echoserver::Handlenewconnection, this, placeholders::_1));
	tcpserver_.Setmessagecallback(bind(&Echoserver::Handlemessage, this, placeholders::_1, placeholders::_2));
	tcpserver_.Setsendcompletecallback(bind(&Echoserver::Handlesendcomplete, this, placeholders::_1));
	tcpserver_.Setclosecallback(bind(&Echoserver::Handleclose, this, placeholders::_1));
	tcpserver_.Seterrorcallback(bind(&Echoserver::Handleerror, this, placeholders::_1));
}

Echoserver::~Echoserver(){}

void Echoserver::Start(){
	tcpserver_.Start();
}

void Echoserver::Handlenewconnection(const spTcpconnection& sptconn){
	cout<< "New Connection come in"<< endl;
}

void Echoserver::Handlemessage(const spTcpconnection& sptcpconn, string& s){
	string msg;
	msg.swap(s);
	msg.insert(0, "reply msg: ");
	sptcpconn->Send(msg);
}

void Echoserver::Handlesendcomplete(const spTcpconnection& sptcpconn){
	cout<< "Messge send conplete"<< endl;
}

void Echoserver::Handleclose(const spTcpconnection& sptcpconn){
	cout<< "Echoserver connet close"<< endl;
}

void Echoserver::Handleerror(const spTcpconnection& sptcpconn){
	cout<< "Echoserver error"<< endl;
}
