#include <sys/epoll.h>
#include "../Pollster.hpp"
namespace Pollster{
	//client member variables
	client::client(int f): fd(f), last_cmd(std::chrono::system_clock::now()){}
	bool client::hasExpired(std::chrono::milliseconds timeout) const{
		auto nw =  std::chrono::system_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(nw-last_cmd) >= timeout; 
	}

	//pollster client variables
	Pollster::Pollster(unsigned int max_clients, const Handler& t) : kq(epoll_create1(0)), clients_max(max_clients), T(t){
		if(kq == -1){
			throw std::runtime_error("Unable to start pollster");
		}
	}
	Pollster::Pollster(Pollster&& other) : kq(std::move(other.kq)), clients(std::move(other.clients)), clients_max(other.clients_max), timeout(other.timeout), T(other.T){
		other.kq = -1;
	}
	Pollster::~Pollster(){
		for(int i = 0; i < clients.size(); i++){
			rmClient(clients[i].fd, "Server is shutting down");
		}
		close(kq);
	}
	void Pollster::cleanup(){
		if(timeout.count() <= 0) return;
		for(int i = 0; i < clients.size(); i++){
			if(clients[i].hasExpired(timeout)){
				rmClient(clients[i].fd, "Timeout");
			}
		}
	}
	bool Pollster::addClient(int fd){
		T.connect(fd);
		struct epoll_event event; 
		event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
 		event.data.fd = fd;	
		if(!epoll_ctl(kq, EPOLL_CTL_ADD, 0, &event)){
			client c(fd);
			clients.push_back(c);
			return true;
		}else{
			return false;
		}
	}

	bool Pollster::rmClient(int fd, std::string reason){
		auto it = std::find(clients.begin(), clients.end(), fd);
		if(it != clients.end()){
			clients.erase(it);
			T.disconnect(fd, reason);
			return true;
		}else{
			return false;
		}
	}


	void Pollster::loop(){
		struct epoll_event evList[32];
		int nev;
	    while(1){
	    	nev = epoll_wait(kq, evList, 32, -1);;
	    	if(nev < 1){
	    		for(int i = 0; i < clients.size(); i++){
	    			T.disconnect(clients[i].fd, "EPoll reported error polling for events");
	    		}
	    		throw std::runtime_error("EPoll reported error polling for events");
	    	}
	    	for(int i = 0; i < nev; i++){
	    		int fd = evList[i].data.fd;
	    		if(evList[i].events & EPOLLRDHUP){
    				auto it = std::find(clients.begin(), clients.end(), fd);
    				if(it != clients.end()){
						clients.erase(it);
					}
					close(fd);
	    		}else{
	    			auto it = std::find(clients.begin(), clients.end(), fd);
	    			it->last_cmd = std::chrono::system_clock::now();
	    			T(fd);
	    		}
	    	}
	    }
	}

}