#include "pch.h"
#include "connection.h"
#include "worker.h"

std::shared_ptr<cli::Connection> cli::Connection::create(std::shared_ptr<cli::Worker> w, boost::asio::io_service& ios, 
		std::string& ip, unsigned int port, std::string handshake_str, int ping_delay, std::string ping_string) {

    return std::shared_ptr<cli::Connection>(new cli::Connection(w, ios, ip, port, handshake_str, ping_delay, ping_string));
}

cli::Connection::Connection(std::shared_ptr<cli::Worker> w, boost::asio::io_service& ios, 
	std::string& ip, unsigned int port, std::string handshake_str, int ping_delay = 0, std::string ping_string = "") :

	worker_(w), 
	ios_(ios), 
	remote_endpoint_(boost::asio::ip::address::from_string(ip), port), 
    socket_(ios, boost::asio::ip::tcp::v4()),
	write_strand_(ios_), 
	showing_message_(false),
    timeout_timer_(ios_), 
	connected_(false), 
	handshake_str_(handshake_str),
	ping_timer_(ios_), 
	ping_delay_(ping_delay), 
	ping_string_(ping_string) {

}

void cli::Connection::start() {
	socket_.connect(remote_endpoint_);

	connected_ = true;

	if (ping_delay_ > 0 && !ping_string_.empty()) {
		ping_timer_.expires_from_now(boost::posix_time::seconds(ping_delay_));
		ping_timer_.async_wait(boost::bind(&cli::Connection::send_ping, this));
	}

	if (!handshake_str_.empty()) {
		send(handshake_str_);
	}

    start_read();
}

void cli::Connection::stop() {
    boost::system::error_code ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    if (ec) {
    }
    socket_.close();
	connected_ = false;
}

boost::asio::ip::tcp::socket& cli::Connection::socket() {
    return socket_;
}

void cli::Connection::start_read() {

    socket_.async_read_some(boost::asio::buffer(buffer_in_), bind(
            &cli::Connection::handle_read,
            shared_from_this(),
            std::placeholders::_1,
            std::placeholders::_2
        )
    );
}

void cli::Connection::handle_read(const boost::system::error_code& ec, const size_t bytes) {
    if (!ec) {
		std::string temp_msg = std::string(buffer_in_.begin(), buffer_in_.begin() + bytes);

		message_buffer_ += temp_msg;

		process_message();

        start_read();
    } else if (boost::asio::error::eof == ec) {
    } else if (boost::asio::error::operation_aborted != ec) {
		this->stop();
    }
}

void cli::Connection::start_write() {
	buffer_out_queue_.front() = msg_begin_str_ + buffer_out_queue_.front() + msg_end_str_;
    boost::asio::async_write(socket_, 
                            boost::asio::buffer(buffer_out_queue_.front()), 
                            write_strand_.wrap([me=shared_from_this()](const boost::system::error_code& ec, std::size_t bytes_sent) {
                                me->handle_write(ec, bytes_sent);
                            }));
}

void cli::Connection::handle_write(const boost::system::error_code& ec, size_t size) {
	if (ec) {
		return;
	}

	buffer_out_queue_.pop_front();

	if (!buffer_out_queue_.empty()) {
		start_write();
	}
}

void cli::Connection::handle_timeout(const boost::system::error_code & ec) {
    if (!ec) {
		this->stop();
    }
}

void cli::Connection::process_message() {
	auto msg_start = message_buffer_.find(msg_begin_str_);
	if (msg_start == std::string::npos) {
		if (message_buffer_.size() > 4096) {
			message_buffer_ = message_buffer_.erase(0, message_buffer_.size() - msg_begin_str_.size());
		}
		return;
	}

	auto msg_end = message_buffer_.find(msg_end_str_);
	if (msg_end == std::string::npos) {
		return;
	}

	std::string complete_message = message_buffer_.substr(msg_start + msg_begin_str_.size(), msg_end - msg_start - msg_begin_str_.size());
	worker_->queue_task(shared_from_this(), complete_message);

	message_buffer_ = message_buffer_.substr(msg_end + msg_end_str_.size());
	process_message();
}

void cli::Connection::send(std::string message) {
	ios_.post(write_strand_.wrap([me=shared_from_this(), message](){
        me->queue_message(message);
    }));
}

void cli::Connection::queue_message(std::string message) {
	bool writing = !buffer_out_queue_.empty();
    buffer_out_queue_.push_back(std::move(message));

    if (!writing) {
        start_write();
    }
}

void cli::Connection::send_msg(std::string message) {
	this->send(message);
}

void cli::Connection::send_ping() {
	send(ping_string_);

	ping_timer_.expires_from_now(boost::posix_time::seconds(ping_delay_));
	ping_timer_.async_wait(boost::bind(&cli::Connection::send_ping, this));
}

