#pragma once

#include "platform.hpp"

#include<list>
#include<vector>
#include<thread>
#include<mutex>

#include<stdint.h>

#include "Frontend.hpp"
#include "Messages.hpp"
#include "Protocol.hpp"
#include "Buffer.hpp"
#include "SocketMessageConnection.hpp"

namespace twili {
namespace twibd {

class Twibd;

namespace frontend {

class SocketFrontend : public Frontend {
	public:
	SocketFrontend(Twibd &twibd, int address_family, int socktype, struct sockaddr *bind_addr, size_t bind_addrlen);
	SocketFrontend(Twibd &twibd, int fd);
	~SocketFrontend();

	class Client : public twibd::Client {
		public:
		Client(SOCKET fd, SocketFrontend &frontend);
		~Client();

		virtual void PostResponse(Response &r) override;

		twibc::SocketMessageConnection connection;
		SocketFrontend &frontend;
		Twibd &twibd;
	};

 private:
	Twibd &twibd;
	SOCKET fd;

	int address_family;
	int socktype;
	struct sockaddr_storage bind_addr;
	size_t bind_addrlen;
	void UnlinkIfUnix();
	
	bool event_thread_destroy = false;
	void event_thread_func();
	std::thread event_thread;
#ifndef _WIN32
	int event_thread_notification_pipe[2];
#endif
	
	std::list<std::shared_ptr<Client>> clients;

	void NotifyEventThread();

	class EventThreadNotifier : public twibc::SocketMessageConnection::EventThreadNotifier {
	 public:
		EventThreadNotifier(SocketFrontend &frontend);
		virtual void Notify() override;
	 private:
		SocketFrontend &frontend;
	} notifier;
};

} // namespace frontend
} // namespace twibd
} // namespace twili
