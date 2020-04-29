#include "pch.h"
#include "worker.h"


cli::Worker::Worker(unsigned int thread_count) : thread_count_(thread_count), work_(ios_) {
    for (size_t i = 0; i < thread_count_; ++i) {
        thread_pool_.emplace_back([=](){ios_.run();});
    }
}

void cli::Worker::queue_task(std::shared_ptr<Connection> cptr, std::string request) {
    ios_.post([=](){process_request(cptr, std::move(request));});
}

