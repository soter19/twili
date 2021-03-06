//
// Twili - Homebrew debug monitor for the Nintendo Switch
// Copyright (C) 2018 misson20000 <xenotoad@xenotoad.net>
//
// This file is part of Twili.
//
// Twili is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Twili is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Twili.  If not, see <http://www.gnu.org/licenses/>.
//

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
	class ServerSocket : public twibc::SocketServer::Socket {
	 public:
		ServerSocket(SocketFrontend &frontend);
		ServerSocket(SocketFrontend &frontend, SOCKET fd);

		ServerSocket &operator=(SOCKET fd);
		
		virtual bool WantsRead() override;
		virtual void SignalRead() override;
		virtual void SignalError() override;
		
	 private:
		SocketFrontend &frontend;
	} server_socket;

	class ServerLogic : public twibc::SocketServer::Logic {
	 public:
		ServerLogic(SocketFrontend &frontend);
		virtual void Prepare(twibc::SocketServer &server) override;
	 private:
		SocketFrontend &frontend;
	} server_logic;

	int address_family;
	int socktype;
	struct sockaddr_storage bind_addr;
	size_t bind_addrlen;
	void UnlinkIfUnix();
	
	std::list<std::shared_ptr<Client>> clients;
	twibc::SocketServer socket_server;
};

} // namespace frontend
} // namespace twibd
} // namespace twili
