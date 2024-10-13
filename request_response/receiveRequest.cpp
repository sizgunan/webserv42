#include "../includes/webserve.hpp"

void reCheckTheServer(configFile &configurationServers, std::string &hostValue, Request &request) {
    try {
        if (request.dup == true) {
            for (const_iterator it = (configurationServers.getServers()).begin(); it != (configurationServers.getServers()).end(); it++) {
                std::map<std::string, std::string>tmp = it->getdirectives();
                if (tmp["server_name"] == hostValue && tmp["listen"] == request.RePort && tmp["host"] == request.ReHost) {
                    std::map<std::string, std::string> serverDirectives = it->getdirectives();
                    std::vector<std::map<std::string, std::string> > serverLocationsBlock = it->getlocationsBlock();
                    request.setDirectivesAndPages(serverDirectives, it->getPages());
                    request.setLocationsBlock(serverLocationsBlock);
                    break ;
                }
            }
        }
    } catch (std::exception &e) {
        request.response = responseBuilder()
            .addStatusLine("400")
            .addContentType("text/html")
            .addResponseBody(request.getPageStatus(400));
        throw "400";
    }
}

bool parseBody(Request &request) {
    request.binaryRead += request.stringUnparsed.length();
    checkLimitRead(request, request.binaryRead);
    if (request.getHttpRequestHeaders()["Transfer-Encoding"] == "chunked")
        requestChunked(request);
    if (!request.getBoundary().empty())  {
        multipartBody(request);
        return false;
    }
    request.setRequestBody(request.stringUnparsed);
    request.stringUnparsed = "";
    return checkLimitRead(request, request.binaryRead);
}

bool parseHeader(std::string &s, Request &request, configFile &configurationServers) {
    (void)configurationServers;
    if (request.stringUnparsed.empty())
        return false;
    if (request.getRequestBodyChunk())
        return parseBody(request);
    std::vector<std::string> lines = customSplitRequest(s, "\r\n");
    for (size_t i = 0; i < lines.size(); i++) {
        if (lines[i] == "\r\n" && !request.getHttpVerb().empty()) {
            validateHeader(request);
            if (request.reCheck != true) {
                request.reCheck = true;
                reCheckTheServer(configurationServers, request.getHttpRequestHeaders()["Host"], request);
            }
            s = "";
            for (size_t j = i + 1; j < lines.size(); j++)
                s += lines[j];
            if (request.getHttpRequestHeaders().count("Content-Length")) {
                if (!checkOverFlow(request.getHttpRequestHeaders()["Content-Length"])) {
                    request.response = responseBuilder()
                        .addStatusLine("413")
                        .addContentType("text/html")
                        .addResponseBody(request.getPageStatus(413));
                    throw "413";
                }
                request.realContentLength = custAtoi(request.getHttpRequestHeaders()["Content-Length"]);
            } else request.realContentLength = 0;
            request.setRequestBodyChunk(true);
            return parseBody(request);
        } else if (lines[i] == "\r\n") {
            continue;
        }
        if (lines[i].find("\r\n") == std::string::npos) {
            s = lines[i];
            return false;
        }
        if ((!request.getHttpVerb().empty() && !parseDefaultLine(lines[i], request))
            || (request.getHttpVerb().empty() && !parseFirstLine(lines[i], request))) {
            return true;
        }
    }
    return false;
}

void receiveRequestPerBuffer(Request &request, int i, configFile &cnf) {
    int recevRequestLen = 0;
    if (FD_ISSET(i, &readsd)) {
        recevRequestLen = recv(i , request.buffer, sizeof(request.buffer), 0);
    } 
    if (recevRequestLen < 0) {
        std::cerr << "Error: recv()" << std::endl;
        throw "REMOVE_THE_CLIENT";
    }
    if (recevRequestLen) {
        if (request.done){
            Request newRequest;
            newRequest.setDirectivesAndPages(request.getDirectives(), request.getPages());
            newRequest.setLocationsBlock(request.getLocationsBlock());
            newRequest.FD = i;
            request = newRequest;
        }
        request.checkTimeout = true;
        request.setTimeout();
        request.stringUnparsed.append(request.buffer, recevRequestLen);
        if (parseHeader(request.stringUnparsed, request, cnf)) {
            request.response = responseBuilder()
                .addStatusLine("400")
                .addContentType("text/html")
                .addResponseBody(request.getPageStatus(400));
            throw "400";
        }
    }
    if ((request.getRequestBodyChunk() && request.binaryRead == request.realContentLength)) {
        request.done = true;
        request.checkTimeout = false;
        request.stringUnparsed = "";
        checkRequestedHttpMethod(request);
    }
}
