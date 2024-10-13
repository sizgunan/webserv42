/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-mal <aait-mal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 18:00:04 by aait-mal          #+#    #+#             */
/*   Updated: 2024/03/13 15:00:13 by aait-mal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cgi.hpp"

void handleTimeout(int signal) {
    // Handle timeout signal
    if (signal == SIGALRM) {
        std::cerr << "Child process timed out and is being terminated.\n";
    }
}

std::map<std::string, std::string> parseHeaders(std::map<std::string, std::string> headers) {
    std::map<std::string, std::string> headerMap;

    for (std::map<std::string, std::string>::iterator entry = headers.begin(); entry != headers.end(); entry++) {
        std::string key = entry->first;
        for (size_t i = 0; i < key.length(); ++i) {
            key[i] = toupper(key[i]);
            if (key[i] == '-')
                key[i] = '_';
        }

        if (key != "HTTP_USER_AGENT" && key != "HTTP_REFERER"
            && key != "CONTENT_LENGTH" && key != "CONTENT_TYPE"
            && key != "AUTH_TYPE" && key != "REMOTE_USER"
            && key != "REMOTE_IDENT" && key != "REQUEST_METHOD"
            && key != "REQUEST_URI" && key != "SERVER_PROTOCOL") {
            key = "HTTP_" + key;
        }

        headerMap[key] = entry->second;
    }
    
    return headerMap;
}

std::map<std::string, std::string> fillEnv(std::map<std::string, std::string>& headers) {
    std::map<std::string, std::string> envVars;

    // Set up the environment variables
    envVars["SERVER_SOFTWARE"] = "webserve";
    envVars["SERVER_NAME"] = "localhost";
    envVars["GATEWAY_INTERFACE"] = "CGI/1.1";
    envVars["SERVER_PROTOCOL"] = "HTTP/1.1";

    envVars["REDIRECT_STATUS"] = "1";

    for (std::map<std::string, std::string>::iterator entry = headers.begin(); entry != headers.end(); entry++) {
        envVars[entry->first] = entry->second;
    }

    return envVars;
}

std::vector<std::string> splitWhiteSpaces(std::string s) {
	std::stringstream ss(s);
	std::vector<std::string> v;
	std::string word;
	while (ss >> word)
		v.push_back(word);
	return (v);
}

bool isValidCGI(std::map<std::string, std::string> &directives, std::string &extension, std::string &cgiPath) {
    if (!directives.count("cgi_bin")) return false;
    std::vector<std::string> cgiParts = splitWithChar(directives["cgi_bin"], '\n');
    for (int i = 0; i < (int)cgiParts.size(); i++) {
        std::vector<std::string> cgiConfig = splitWhiteSpaces(cgiParts[i]);
        if (cgiConfig.size() < 2) continue;
        if (access(cgiConfig[0].c_str(), F_OK | X_OK) == -1) continue;
        for (int i = 1; i < (int)cgiConfig.size(); i++)
            if (cgiConfig[i] == extension) return (cgiPath = cgiConfig[0], true);
    }
    return false;
}
