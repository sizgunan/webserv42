#include "../includes/webserve.hpp"


configFile::configFile() {}


const std::vector<Server>& configFile::getServers() const {
    return servers;
}

void configFile::setServers(Server &initiateServer) {
    servers.push_back(initiateServer);
}

void configFile::setTheVector( std::vector<Server> servers) {
    this->servers = servers;
}
configFile::~configFile() {}
