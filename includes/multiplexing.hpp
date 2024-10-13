#pragma once


#include "Server.hpp"
#include "configFile.hpp"
// #include <sys/_select.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>

#define FOREVER ;;


typedef  std::vector<Server>::const_iterator const_iterator;
typedef  std::vector<int>::const_iterator socket_iterator;

