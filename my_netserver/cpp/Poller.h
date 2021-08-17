#ifndef _POLLER_H_
#define _POLLER_H_

#include<vector>
#include<mutex>
#include<map>
#include<sys/epoll.h>
#include"Channel.h"
using namespace std;

class Poller {

	public:
		typedef vector<Channel*> ChannelList;
		
		int pollfd_;
		
		vector<struct epoll_event> eventlist_;
		
		map<int, Channel*> channelmap_;
		
		mutex mutex_;
		
			
		Poller();
		~Poller();
		
		void poll(ChannelList & activechannellist);
		
		void AddChannel(Channel* pchannel);
		
		void RemoveChannel(Channel* pchannel);
		
		void UpdateChannel(Channel* pchannel);
		
};


#endif
