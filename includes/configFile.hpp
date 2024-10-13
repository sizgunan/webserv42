#pragma once

#include "Server.hpp"
#include <iostream>
#include <string>
#include <vector>

class configFile {

private:

    std::vector<Server> servers;


public:

    configFile();

    const std::vector<Server> &getServers() const ;
    void setServers(Server &initiateServer);
    void setTheVector( std::vector<Server> servers);

    ~configFile();

};