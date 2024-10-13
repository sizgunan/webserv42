#pragma once

#include "webserve.hpp"

class Pipe {
	private:
		int pipefd[2];

	public:
		Pipe();
		~Pipe();
		int getReadEnd();
		int getWriteEnd();
};

void redirectStdoutStderr(Pipe& pipe);
std::string readFromPipeAndClose(int readEnd);
int executeChildProcess(const std::string& interpreter, const std::string& scriptFilePath, std::map<std::string, std::string>& envVars);
std::vector<std::string> splitWithString(const std::string& s, const std::string& delimiter);
std::pair<std::string, std::string> splitHeadersAndBody(const std::string& response);
void handleTimeout(int signal);

std::map<std::string, std::string> parseHeaders(std::map<std::string, std::string> headers);
std::map<std::string, std::string> fillEnv(std::map<std::string, std::string>& headers);

std::pair<std::string, std::string> handleCgiGet(const std::string& file, const std::string& interpreterPath, Request &request);
std::pair<std::string, std::string> handleCgiPost(const std::string& file, const std::string& interpreterPath, Request &request);

bool isValidCGI(std::map<std::string, std::string> &directives, std::string &extension, std::string &cgiPath);
std::string extractContentType(const std::string& headers);
std::multimap<std::string, std::string> parseResponseHeaders(const std::string& headers);
std::vector<std::string> splitWhiteSpaces(std::string s);