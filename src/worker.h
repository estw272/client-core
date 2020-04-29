#pragma once

#include "connection.h"

namespace cli {
	class Worker;
}

class cli::Worker {
public:
    explicit Worker(unsigned int thread_count = 1); 

    virtual void queue_task(std::shared_ptr<cli::Connection> cptr, std::string request);

    virtual ~Worker() = default;

private:
    virtual void process_request(std::shared_ptr<cli::Connection> cptr, std::string request) = 0;

private:
    unsigned int                  thread_count_;
    std::vector<std::thread>      thread_pool_;
    boost::asio::io_service       ios_;
    boost::asio::io_service::work work_;

};

