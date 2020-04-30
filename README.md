# client-core

## About
Boost::Asio client static library

## Dependencies
Spdlog  
Boost (tested on boost 1.72.0)

## Usage
```c++
#include "clientcore/client.h"

int main() {
    std::string ping_str = "ping string";
    std::string handshake_str = "handshake string";
    const int ping_delay = 10;

    //Child is a class, derived from cli::Worker
    std::shared_ptr<cli::Worker> worker = std::make_shared<Child>(2);

    cli::Client cl(worker, 2);
    cl.set_handshake(handshake_str);
    cl.set_ping(ping_str);
    cl.set_ping_delay(ping_delay);
    cl.keep_alive(true);
    cl.connect("127.0.0.1", 1234);
}
```
