#ifndef CONNECTION_H_23092323
#define CONNECTION_H_23092323

namespace cli {
	class Connection;
	class Worker;
}

class cli::Connection : public std::enable_shared_from_this<Connection> {
private: //#TODO: cleanup
	std::shared_ptr<cli::Worker>				 worker_;
	boost::asio::io_service&                     ios_;
	boost::asio::ip::tcp::socket                 socket_;
	boost::asio::io_service::strand              write_strand_;
	unsigned int                                 buffer_len_ = 4096;
	std::array<char, 4096>                       buffer_in_;
	boost::asio::streambuf                       buffer_out_;
	boost::asio::streambuf                       packet_in_;
	boost::asio::streambuf::mutable_buffers_type read_packet_in_ = packet_in_.prepare(4096);
	
	std::deque<std::string> buffer_out_queue_;

	bool showing_message_;

	std::string prev_message_;
	std::string message_buffer_ {};

	boost::asio::deadline_timer timeout_timer_;

	boost::asio::ip::tcp::endpoint remote_endpoint_;

	bool connected_;

	std::string					  handshake_str_;
	boost::asio::deadline_timer   ping_timer_;
	int                           ping_delay_;
	std::string                   ping_string_;

	const std::string msg_begin_str_ = "__MSG_STS__";
	const std::string msg_end_str_   = "__MSG_STE__";

public:
	Connection(const Connection &) = delete;
	Connection& operator=(const Connection &) = delete;

	static std::shared_ptr<Connection> create(std::shared_ptr<cli::Worker> w, boost::asio::io_service& ios, 
		std::string& ip, unsigned int port, std::string handshake_str, int ping_delay = 0, std::string ping_string = "");

	void start();
	void stop();
	boost::asio::ip::tcp::socket& socket();

	void send_msg(std::string message);

private:
	explicit Connection(std::shared_ptr<cli::Worker> w, boost::asio::io_service&, std::string& ip, unsigned int port, 
		std::string handshake_str, int ping_delay, std::string ping_string);

	void start_read();
	void handle_read(const boost::system::error_code &, const size_t);
	void start_write();
	void handle_write(const boost::system::error_code &, const size_t);
	void handle_timeout(const boost::system::error_code &);
	void process_message();
	void send(std::string message);
	void queue_message(std::string message);
	void send_ping();

};

#endif 