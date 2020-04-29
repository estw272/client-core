
#pragma once
#ifndef _PCHCLI_H_
#define _PCHCLI_H_

#define WIN32_LEAN_AND_MEAN
#define BOOST_ASIO_DISABLE_CONCEPTS

// std
#include <iostream>
#include <filesystem>
#include <thread>
#include <vector>
#include <memory>
#include <queue>
#include <deque>
#include <functional>
#include <sstream>
#include <fstream>
#include <mutex>
#include <set>
#include <algorithm>
#include <optional>
#include <atomic>

// diff
#include "boost/asio.hpp"
#include "boost/bind.hpp"

//winapi
#include <WS2tcpip.h>
#include <windows.h>

//my libs
//#include "../include/clientcore/client.h"

#endif
