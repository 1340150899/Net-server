#include<signal.h>
#include"EventLoop.h"
#include"EchoServer.h"

EventLoop *lp;

static void sighandler1(int sig_no){
	exit(0);
}

static void sighandler2(int sig_no){
	lp->Quit();
}

int main(int argc, char *argv[]){
	
	signal(SIGUSR1, sighandler1);
	
	signal(SIGUSR2, sighandler2);
	
	signal(SIGINT, sighandler2);
	
	signal(SIGPIPE, SIG_IGN); 
	
	int port = 6666;
	
	int iothreadnum = 1;
	
	int workerthreadnum = 0;
	if(argc == 4){
		port = atoi(argv[1]);
		iothreadnum = atoi(argv[2]);
		workerthreadnum = atoi(argv[3]);
	}
//	在EventLoop的构造函数中会初始化一个poller类变量而这个poller对象
//	会初始化一个epollfd 之后EventLoop会创建一个 eventfd这个fd就是常
//	规epoll模型中的文件fd 只不过因为是多线程所以用eventfd来进行多线
//	程交互 eventfd会存在EventLoop中的 Channel wakeupchannel 之后会
//	用EventLoop中的read、error成员函数bind到Channel类的 回调函数中 
//	之后将这个 wakeupchannel add 到 EventLoop初始化的poller_中 在poller_的add
//	中会根据wakeuochannel中的类成员变量新建一个struct epoll_event 
//	变量并将 这个epoll_event 通过epoll_ctl库函数add到监听的列表中这就是epoll的i/o复用  
	
	EventLoop loop;
	lp = &loop;
	
	
	
	Echoserver echoserver(&loop, port, iothreadnum);
	echoserver.Start();
	
	
	lp->loop();
	
	
	return 0;
}
