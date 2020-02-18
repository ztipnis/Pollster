#include <unistd.h>
#include <chrono>
#include <ctime>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#ifndef __POLLSTER__H__
#define __POLLSTER__H__

namespace Pollster{
	class Pollster;
	class client{
	public:
		int fd;
		std::chrono::time_point<std::chrono::system_clock> last_cmd;
		explicit client(int f);
		bool operator==(int f) const{ return fd == f;}
		bool hasExpired(std::chrono::milliseconds timeout) const;
	};


	class Handler{
	public:
		virtual void operator()(int fd) const = 0;
		virtual void disconnect(int fd, const std::string &reason) const = 0;
		virtual void connect(int fd) const = 0;
	};

	class Pollster{
	public:
		Pollster(unsigned int max_clients, const Handler& t);
		Pollster(Pollster&& other);
		~Pollster();
		void operator()() { loop();  }
		Pollster& operator=(const Pollster& p) = delete;
		bool canAddClient() const{ return clients.size() < clients_max;}
		bool addClient(int fd);
		bool rmClient(int fd, std::string reason);
		void setTimeout(std::chrono::milliseconds tout){ timeout = tout; }
		void cleanup();
	private:
		int kq;
		std::vector<client> clients;
		unsigned int clients_max;
		std::chrono::milliseconds timeout;
		const Handler& T;
		void loop();
	};

}
#endif