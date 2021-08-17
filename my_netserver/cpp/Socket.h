#ifndef _SOCKET_H_
#define _SOCKET_H_

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

class Socket {
	private:
		
		int serverfd_;
		
	public:
	
		Socket();
		~Socket();
		
		int fd()const{
			return serverfd_;
		}
		
		void Setsocketoption();

		void Setreuseaddr();
		
		void Setnonblocking();
		
		bool Bindaddress(int serverport);
		
		bool Listen();
		
		int Accept(struct sockaddr_in& clientaddr);
		
		bool Close();
};

#endif
