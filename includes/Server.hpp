#pragma once


#include <string>
#include <iostream>
#include <map>
#include <stack>
#include <netinet/in.h>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <fstream>
#include <sstream>


class Server {

private:

    int socketD;
    struct sockaddr_in serverAddress;
    std::map<std::string, std::string> directives;
    std::vector<std::map<std::string, std::string> > locationsBlock;
    std::map<std::string, std::map<int, std::string> > pages;

public:

    std::string preHost;
    std::string prePort;
    bool duplicated;

    Server();
    void bindSockets();
    void listenToIncomingConxs();

    int getSocketDescriptor() const;
    std::map<std::string, std::string> const& getdirectives(void) const ;
    std::map<std::string, std::map<int, std::string> > const& getPages(void) const ;
    std::vector<std::map<std::string, std::string> > const& getlocationsBlock(void) const;


    void setServerAddress(struct sockaddr_in &eachServerAddress);
    void setSocketDescriptor(int &socketDescriptor);
    void setDirectives(std::map<std::string, std::string> &newDirectives);
    void setLocationBlock(std::vector<std::map<std::string, std::string> > &newLocDirectives);

    static std::vector<Server> parsingFile(std::string s);
    static void overrideLocations(Server &s);
    static void fillErrorPages(Server &s);
    static void runServers(std::vector<Server> &servers);

    ~Server();
};
