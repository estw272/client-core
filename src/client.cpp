#include "pch.h"
#include "../include/clientcore/client.h"


cli::Client::Client(std::shared_ptr<cli::Worker> w, unsigned int num_threads) : 
	thread_count_(num_threads), 
	work_(ios_), 
	worker_(w),
	handshake_str_(""), 
	ping_str_(""), 
	ping_delay_(0), 
	keep_alive_(false) {
}

cli::Client::~Client() {
	conn_ptr_->stop();
}

void cli::Client::connect(std::string ip, unsigned int port) {
	for (unsigned int i = 0; i < thread_count_ - static_cast<int>(keep_alive_); ++i) {
		thread_pool_.emplace_back([me=this](){
			me->ios_.run();
		});
	}

	conn_ptr_ = cli::Connection::create(worker_, ios_, ip, port, handshake_str_, ping_delay_, ping_str_);
	conn_ptr_->start();

	if (keep_alive_) {
		ios_.run();
	}
}

void cli::Client::send(std::string message) {
	conn_ptr_->send_msg(message);
}

void cli::Client::set_handshake(std::string& handshake_str) {
	handshake_str_ = handshake_str;
}

void cli::Client::set_ping(std::string& ping_str) {
	ping_str_ = ping_str;
}

void cli::Client::set_ping_delay(unsigned int ping_delay) {
	ping_delay_ = ping_delay;
}

void cli::Client::keep_alive(bool keep) {
	keep_alive_ = keep;
}
