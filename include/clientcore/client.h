#pragma once
#ifndef _CLIENT_H_8768765
#define _CLIENT_H_8768765

#include <queue>
#include "connection.h"
#include "worker.h"

namespace cli {
	class Client;
}

class cli::Client {
private:
	unsigned int                  thread_count_;
	std::vector<std::thread>      thread_pool_;
	boost::asio::io_service       ios_;
	boost::asio::io_service::work work_;

	std::shared_ptr<Worker>     worker_;
	std::shared_ptr<Connection> conn_ptr_;

	std::string handshake_str_;
	std::string ping_str_;
	int         ping_delay_;
	bool        keep_alive_;

public:
	explicit Client(std::shared_ptr<cli::Worker> w, unsigned int num_threads=1);
	~Client();

	Client(const Client&) = delete;
	Client& operator=(const Client&) = delete;
	Client(Client&&) = delete;
	Client& operator=(Client&&) = delete;

	void set_handshake(std::string& handshake_str);
	void set_ping(std::string& ping_str);
	void set_ping_delay(unsigned int delay);
	void keep_alive(bool keep);

	void connect(std::string ip, unsigned int port);
	void send(std::string message);
};

#endif

