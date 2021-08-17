#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include<cstdint>
#include<functional>

class Channel {
		
	public:
		
		typedef std::function<void()> Callback;
		Channel();
		~Channel();
		
		void SetFd(int fd){
			fd_ = fd;
		}
		
		int Getfd(){
			return fd_;
		}
		
		void SetEvents(uint32_t events){
			events_ = events;
		}
		
		uint32_t Getevents() const{
			return events_;
		}
		
		void HandleEvent();
		
		void SetReadHandle(const Callback &cb){
			readhandler_= cb;
		}
		
		void SetWriteHandle(const Callback &cb){
			writehandler_ = cb;
		}
		
		void SetErrorHandle(const Callback &cb){
			errorhandler_ = cb;
		}
		
		void SetCloseHandle(const Callback &cb){
			closehandler_ = cb;
		}
	private:
		int fd_;
		
		uint32_t events_;
		
		Callback readhandler_;
		Callback writehandler_;
		Callback errorhandler_;
		Callback closehandler_;				
};

#endif
