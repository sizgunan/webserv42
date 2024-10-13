/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   postMethod.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obelaizi <obelaizi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 01:42:14 by obelaizi          #+#    #+#             */
/*   Updated: 2024/03/31 01:42:15 by obelaizi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserve.hpp"

void postFolder(std::string &root, std::string &uri, Request &request) {

    if ( !request.getSaveLastBS() ) {
        request.response = responseBuilder()
            .addStatusLine("301")
            .addContentType("text/html")
            .addLocation(uri)
            .addResponseBody("<html><h1>301 Moved Permanently</h1></html>");
        throw "301";
    }

    std::map<std::string, std::string> directives = request.getLocationBlockWillBeUsed();
    mapConstIterator it = directives.find("index");

    if (it != directives.end()) {
        std::string indexFile = it->second;

        // Remove extra "/" from start of root
        for (size_t i = 0; i < root.length(); i++) {
            if (root[i] == '/' && root[i+1] && root[i+1] == '/') {
                root = root.erase(i, 1);
            }
        }

        // If index file contains the root directory in it remove it
        if (indexFile.find(root) != std::string::npos) {
            indexFile = indexFile.substr(root.length());
        }

        std::string absolutePath = "";
        std::vector<std::string> splitedPaths;

        // Insert all index files splitted by space into a vector
        std::istringstream iss(indexFile);
        std::string token;
        while (std::getline(iss, token, ' ')) {
            splitedPaths.push_back(token);
        }

        // Decide which index file to use
        for (size_t i = 0; i < splitedPaths.size(); i++) {
            std::fstream file((root + '/' + splitedPaths[i]).c_str());
            if ( file.good() ) {
                absolutePath = CheckPathForSecurity(root+'/'+splitedPaths[i]);
                break;
            }
        }

        if (!absolutePath.empty()) {

            std::pair<std::string, std::string> response;
            std::string extension = absolutePath.substr(absolutePath.find_last_of('.'));

            std::map<std::string, std::string> locationBlock = request.getLocationBlockWillBeUsed();
            std::string binaryPath;

            if (isValidCGI(locationBlock, extension, binaryPath)) {

                response = handleCgiPost(absolutePath, binaryPath, request);
                std::string headers = response.first;
                std::string body = response.second;

                std::string contentType = extractContentType(headers);
                std::string contentLength = ftToString(body.length());

                std::size_t lastSlashPos = contentType.rfind('/');
                std::string type = (lastSlashPos != std::string::npos) ? contentType.substr(lastSlashPos + 1) : "txt";

                std::multimap<std::string, std::string> splitedHeaders = parseResponseHeaders(headers);

                request.response = responseBuilder()
                    .addStatusLine("200")
                    .addContentType(type);
                for (std::multimap<std::string, std::string>::iterator it = splitedHeaders.begin(); it != splitedHeaders.end(); it++) {
                    if (it->second.find("\r") != std::string::npos) {
                        it->second = it->second.substr(0, it->second.find("\r"));
                    }
                    if (it->second.find("\n") != std::string::npos) {
                        it->second = it->second.substr(0, it->second.find("\n"));
                    }

                    if (it->first == "Status") {
                        std::string status = it->second;
                        std::stringstream ss(status);
                        std::string statusNumber;
                        ss >> statusNumber;

                        request.response.addStatusLine(statusNumber);
                    } else {
                        std::string key = lower(it->first);
                        if (key != "content-type" && key != "content-length")
                            request.response.addCustomHeader(it->first, it->second);
                    }
                }
                request.response.addResponseBody(body);

                throw ("CGI");
            }
        }
    }
    request.response = responseBuilder()
        .addStatusLine("403")
        .addContentType("text/html")
        .addResponseBody(request.getPageStatus(403));
    throw ("403");
}

void postFile(std::string &absolutePath, std::string &uri, Request &request) {

    std::pair<std::string, std::string> response;
    size_t pos = uri.rfind('/');
    std::string file = uri.erase(0, pos);
    if ( file.find('.') != std::string::npos ) {

        std::string extension = file.substr(file.find_last_of('.'));
        std::map<std::string, std::string> locationBlock = request.getLocationBlockWillBeUsed();
        std::string binaryPath;

        if (isValidCGI(locationBlock, extension, binaryPath)) {

            response = handleCgiPost(absolutePath, binaryPath, request);
            std::string headers = response.first;
            std::string body = response.second;

            std::string contentType = extractContentType(headers);
            std::string contentLength = ftToString(body.length());

            std::size_t lastSlashPos = contentType.rfind('/');
            std::string type = (lastSlashPos != std::string::npos) ? contentType.substr(lastSlashPos + 1) : "txt";

            std::multimap<std::string, std::string> splitedHeaders = parseResponseHeaders(headers);

            // Set the initial HTTP response headers
            request.response = responseBuilder()
                .addStatusLine("200")
                .addContentType(type);
            for (std::multimap<std::string, std::string>::iterator it = splitedHeaders.begin(); it != splitedHeaders.end(); it++) {
                if (it->second.find("\r") != std::string::npos) {
                    it->second = it->second.substr(0, it->second.find("\r"));
                }
                if (it->second.find("\n") != std::string::npos) {
                    it->second = it->second.substr(0, it->second.find("\n"));
                }

                if (it->first == "Status") {
                    std::string status = it->second;
                    std::stringstream ss(status);
                    std::string statusNumber;
                    ss >> statusNumber;

                    request.response.addStatusLine(statusNumber);
                } else {
                    std::string key = lower(it->first);
                    if (key != "content-type" && key != "content-length")
                        request.response.addCustomHeader(it->first, it->second);
                }
            }
            request.response.addResponseBody(body);

            throw ("CGI");
        }
    }
    request.response = responseBuilder()
        .addStatusLine("403")
        .addContentType("text/html")
        .addResponseBody(request.getPageStatus(403));
    throw "403";

}
