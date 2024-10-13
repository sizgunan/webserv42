#include "../includes/webserve.hpp"

Server::Server() :
    duplicated(false) {}


void Server::setSocketDescriptor(int &socketDescriptor) {
    this->socketD = socketDescriptor;
}



void Server::setDirectives(std::map<std::string, std::string> &newDirectives) {
    this->directives = newDirectives;
}


void Server::bindSockets() {

    if (bind(socketD, (struct sockaddr *)&(serverAddress), sizeof(serverAddress)) < 0) {
        std::cerr << "Error: bind() " << std::endl;
        exit (1);
    }

}

void Server::listenToIncomingConxs() {

    if (listen(socketD, SOMAXCONN) < 0) {
        std::cerr << "Error: listen() " << std::endl;
        exit (1);
    }
}


int Server::getSocketDescriptor() const {
    return (socketD);
}

std::map<std::string, std::string> const& Server::getdirectives(void) const {
    return (this->directives);
}


std::vector<std::map<std::string, std::string> > const& Server::getlocationsBlock(void) const {
    return (this->locationsBlock);
}

void Server::setServerAddress(struct sockaddr_in &eachServerAddress) {
    this->serverAddress = eachServerAddress;
}


void Server::setLocationBlock(std::vector<std::map<std::string, std::string> > &newLocDirectives) {
    this->locationsBlock = newLocDirectives;
}

std::map<std::string, std::map<int, std::string> > const& Server::getPages(void) const {
    return (this->pages);
}

Server::~Server() {}