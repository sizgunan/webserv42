/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obelaizi <obelaizi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 18:35:45 by obelaizi          #+#    #+#             */
/*   Updated: 2024/04/04 23:30:03 by obelaizi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserve.hpp"

using namespace std;


bool checkReturnOnLocation(vector<map<string, string> > &locationsBlock) {
	for (size_t i = 0; i < locationsBlock.size(); i++) {
		if (!locationsBlock[i].count("return"))
			continue;
		std::string returnStr = locationsBlock[i]["return"];
		vector<string> v = splitWhiteSpaces(returnStr);
		if (v.size() != 2)
			return (false);
		if (v[0].size() > 3 || v[0].size() == 0)
			return (false);
		for (size_t i = 0; i < v[0].size(); i++)
			if (!isdigit(v[0][i]))
				return (false);
		int statusCode = atoi(v[0].c_str());
		if (statusCode < 300 || statusCode > 399)
			return (false);
	}
	return (true);
}

bool checkDuplicateLocation(vector<map<string, string> > &locationsBlock) {
	set<string> locationSet;
	for (size_t i = 0; i < locationsBlock.size(); i++) {
		if (locationSet.count(locationsBlock[i]["location"])) {
			return (true);
		}
		locationSet.insert(locationsBlock[i]["location"]);
	}
	return (false);
}

bool checkPortMaxMin(string port) {
	if (port.size() > 5 || port.size() == 0)
		return (false);
	for (size_t i = 0; i < port.size(); i++)
		if (!isdigit(port[i]))
			return (false);
	int portInt = atoi(port.c_str());
	if (portInt < 1025 || portInt > 65535)
		return (false);
	return (true);
}

void	GetDirectives(string &word, map<string, string> &directives, string &key) {
	if (key == "")
		key = word;
	else {
		if (*word.rbegin() == ';')
			word.erase(word.size() - 1);
		directives[key] = word;
	}

}

string convertDomainToIPv4(string &domain)
{
    struct addrinfo hints, *result, *p;
    char ipstr[INET_ADDRSTRLEN];
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(domain.c_str(), NULL, &hints, &result) != 0)
        return "";
	std::string ret ="";
    for (p = result; p != NULL; p = p->ai_next)
    {
        if (p->ai_family == AF_INET)
        {
            struct sockaddr_in *ipv4 = reinterpret_cast<struct sockaddr_in *>(p->ai_addr);
            inet_ntop(AF_INET, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
			ret = ipstr;
            break;
        }
    }
    freeaddrinfo(result);
    return ret;
}

bool checkOverFlow(std::string &s) {
	if (s.size() > 19)
		return (false);
	for (size_t i = 0; i < s.size(); i++)
		if (!isdigit(s[i]))
			return (false);
	return (true);

}

void Server::overrideLocations(Server &s) {
	map<string, string> ServerDirectives = s.getdirectives();
	for (size_t i=0;i<s.locationsBlock.size();i++) {
		map<string, string> &location = s.locationsBlock[i];
		for (std::map<std::string, std::string>::iterator j = ServerDirectives.begin(); j != ServerDirectives.end(); j++)
			if (!location.count(j->first) && j->first != "listen" && j->first != "host")
				location[j->first] = j->second;
		if (!location.count("client_max_body_size")) continue;
		std::string maxClientBodySize = location["client_max_body_size"];
		if (!checkOverFlow(maxClientBodySize)) {
			throw runtime_error("Error: Invalid client_max_body_size on location " + location["location"]);
		}
	}
}

void adjustServerAddress(Server &server, struct sockaddr_in &serverAddress) {

    bzero(&serverAddress, sizeof(serverAddress));

    int port = atoi(((server.getdirectives().find("listen"))->second).c_str());
	server.prePort = (server.getdirectives().find("listen")->second);
    serverAddress.sin_family = AF_INET;
	string host= ((server.getdirectives().find("host"))->second);
	server.preHost = host;
	string ultimateHost = convertDomainToIPv4(host);
	if (ultimateHost.empty()) {
		cerr << "Invalid convert Domain To IPv4\n";
		exit (1);
	}
	serverAddress.sin_addr.s_addr = inet_addr(ultimateHost.c_str());
    serverAddress.sin_port = htons(port);
}

void Server::runServers(std::vector<Server> &servers) {
	std::set<std::pair<std::string, std::string> > Check;
	for (size_t i = 0; i < servers.size(); i++) {
		if (servers[i].directives.count("host") && servers[i].directives["host"] == "127.0.0.1")
			servers[i].directives["host"] = "localhost";
		for (size_t j = 0; j < servers.size(); j++) {
			if (i == j) continue;
			if (servers[i].directives["listen"] == servers[j].directives["listen"]
				&& servers[i].directives["host"] == servers[j].directives["host"] && servers[i].directives["server_name"] == servers[j].directives["server_name"])
			throw runtime_error("Error: Duplicate servers");
			if (servers[i].directives["listen"] == servers[j].directives["listen"]
				&& servers[i].directives["host"] == servers[j].directives["host"])
				servers[i].duplicated = true, servers[j].duplicated = true;
		}
		adjustServerAddress(servers[i], servers[i].serverAddress);
		servers[i].setServerAddress(servers[i].serverAddress);


		if (( servers[i].socketD = socket(AF_INET, SOCK_STREAM, 0) ) < 0) {
			std::cerr <<"Error: Fail to create a Socket for Server 1\n";
			exit(1);
		}
		int add = 1;
		setsockopt(servers[i].socketD, SOL_SOCKET, SO_REUSEADDR, &add, sizeof(add));
		servers[i].setSocketDescriptor(servers[i].socketD);
		//* SAVE HISTORY ( HOST & PORT )
		if (!Check.count(std::make_pair(servers[i].directives["listen"], servers[i].directives["host"]))) {
			servers[i].bindSockets();
			servers[i].listenToIncomingConxs();
			Check.insert(std::make_pair(servers[i].directives["listen"], servers[i].directives["host"]));
		}
	}
	// set index.html if index is empty, and remove autoindex if it's off
	for (size_t i = 0; i < servers.size(); i++) {
		for (size_t j = 0; j < servers[i].directives.size(); j++) {
			if (servers[i].directives.count("autoindex") && servers[i].directives["autoindex"] == "off")
				servers[i].directives.erase("autoindex");
		}
		overrideLocations(servers[i]);
	}
}

vector<Server> Server::parsingFile(string s) {
	stack<string> st;
	vector<Server> servers;
	Server server;
	map<string, string> directives;
	vector<map<string, string> > locationsBlock;
	string line;
	ifstream file(s.c_str());
	int lineNum = 0;
	if (file.is_open()) {
		while (getline(file, line)) {
			stringstream lineNumStr;
			lineNum++;
			lineNumStr << lineNum;
			vector<string> v = splitWhiteSpaces(line);
			if (v.size() == 0 || v[0][0] == '#')
				continue;
			if (v.size() == 1 && *v[0].rbegin() != '{' && *v[0].rbegin() != '}')
				throw runtime_error("Syntax error");
			if (*v.rbegin()->rbegin() == '{') {
				v.rbegin()->erase(v.rbegin()->size() - 1);
				if (*v.rbegin() == "") v.erase(v.end() - 1);
				if (v.empty()) throw runtime_error("Syntax error");
				if (v.size() == 1 && v[0] == "server") { // on server brackets
					if (!st.empty()) throw runtime_error("Can't have a block inside a block");
					st.push(v[0]);
					locationsBlock.clear();
					directives.clear();
					continue;
				}
				if (v.size() != 2)
					throw runtime_error("Error: wrong number of arguments on " + v[0] + " line " + lineNumStr.str() );
				if (st.empty())
					throw runtime_error("Error: The location on line "+ lineNumStr.str() +" block should be inside server Block");
				if (st.top() == "location")
					throw runtime_error("Error: Can't have a block inside a block");
				st.push(v[0]);// on location brackets
				if (server.directives.empty())
					server.directives = directives;
				directives.clear();
				directives[v[0]] = v[1];
				continue;
			}
			if (v.size() == 1 && v[0] == "}") {
				if (st.empty()) throw runtime_error("Error: } without {");
				if (st.top() == "server") {
					server.directives.insert(directives.begin(), directives.end());
					server.locationsBlock = locationsBlock;
					servers.push_back(server);
					directives.clear();
					locationsBlock.clear();
					server.directives.clear();
					server.locationsBlock.clear();
				} else if (st.top() == "location") {
					locationsBlock.push_back(directives);
					directives.clear();
				}
				st.pop();
				continue;
			}
			if (*v.rbegin()->rbegin() == ';') v.rbegin()->erase(v.rbegin()->size() - 1);
			else throw runtime_error("Error: missing \";\" on line " + lineNumStr.str());
			if (*v.rbegin() == "") v.erase(v.end() - 1);
			if ((v[0] == "host" || v[0] == "listen" || v[0] == "server_name") && (st.top() == "location")) {
				throw runtime_error("Error: on line " + lineNumStr.str()  + " \"" +  v[0] + "\" can't be inside location block");
			}
			if (v[0] == "location")
				throw runtime_error("Error: location shouldn't be directive on line " + lineNumStr.str());
			if ((v[0] == "cgi_bin" || v[0] == "error_page") && directives.count(v[0]))
                directives[v[0]] += '\n' + v[1];
			else directives[v[0]] = v[1];
			if (v[0] == "root" && v.size() > 2)
				throw runtime_error("Error: root should only have one argument on line " + lineNumStr.str());
			for (size_t i = 2; i < v.size(); i++) {
				if (*v[i].rbegin() == ';')
					v[i].erase(v[i].size() - 1);
				if (v[i] == "") continue;
				directives[v[0]] += " " + v[i];
			}
		}
		file.close();
	}
	if (!st.empty())
		throw runtime_error("Error: { without }");
	try {
		stringstream lineNumStr;
		for (size_t i = 0; i < servers.size(); i++) {
			lineNumStr << i + 1;
			if (!checkPortMaxMin(servers[i].directives["listen"]))
				throw runtime_error("Error: Invalid port number on server Num " + lineNumStr.str());
			if (checkDuplicateLocation(servers[i].locationsBlock))
				throw runtime_error("There is a duplicate Location on server Num " + lineNumStr.str());
			if (!checkReturnOnLocation(servers[i].locationsBlock))
				throw runtime_error("Error: Invalid return on server Num " + lineNumStr.str());
			fillErrorPages(servers[i]);
		}
		runServers(servers);
	} catch (runtime_error &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	return (servers);
}
