#include "../includes/webserve.hpp"

void    fillFirstPartOfMultipart(Request &request) {
    size_t pos = request.stringUnparsed.find("\r\n\r\n");
    if (pos == std::string::npos) {
        request.firstPart += request.stringUnparsed;
        request.stringUnparsed = "";
        return;
    }
    request.firstPart += request.stringUnparsed.substr(0, pos + 4);
    request.stringUnparsed.erase(0, pos + 4);
    std::string &s = request.firstPart;
    std::string tmp = request.getBoundary() + "\r\nContent-Disposition: form-data; name=\"";
    for (size_t i = 0; i < tmp.length(); i++) {
        if (s[i] != tmp[i]) {
            request.response = responseBuilder()
            .addStatusLine("400")
            .addContentType("text/html")
            .addResponseBody(request.getPageStatus(400));
            throw "400";
        }
    }
    pos = s.find("\"; filename=\"");
    if (pos == std::string::npos) {
        request.response = responseBuilder()
        .addStatusLine("400")
        .addContentType("text/html")
        .addResponseBody(request.getPageStatus(400));
        throw "400";
    }
    pos += 13;
    size_t pos2 = s.find("\"", pos);
    if (pos2 == std::string::npos) {
        request.response = responseBuilder()
        .addStatusLine("400")
        .addContentType("text/html")
        .addResponseBody(request.getPageStatus(400));
        throw "400";
    }
    tmp = "\"\r\nContent-Type: ";
    request.fileName = request.getLocationBlockWillBeUsed().find("upload_store")->second + "/" + s.substr(pos, pos2 - pos);
    for (size_t i = 0;i < tmp.length(); i++) {
        if (tmp[i] != s[pos2 + i]) {
            request.response = responseBuilder()
                .addStatusLine("400")
                .addContentType("text/html")
                .addResponseBody(request.getPageStatus(400));
            throw "400";
        }
    }
    pos = request.fileName.find_last_of(".");
    if (pos == std::string::npos)
        pos = request.fileName.length();
    while (std::ifstream(request.fileName.c_str()))
        request.fileName.insert(pos, "_");
}

bool isGoodFirstPart(std::string &s) {
    return (s.length() >= 4 && s.substr(s.length() - 4) == "\r\n\r\n");
}

void multipartBody(Request &request) {
    try {
        if (!isGoodFirstPart(request.firstPart)) {
            fillFirstPartOfMultipart(request);
        }
        const std::string  &boundary = request.getBoundary();
        if (request.stringUnparsed.empty())
            return;

        if (request.stringUnparsed.length() == boundary.length() + 6) {
            swap(request.lastBoundary, request.stringUnparsed);
        } else if (request.stringUnparsed.length() < boundary.length() + 6) {
            request.lastBoundary += request.stringUnparsed;
            if (request.lastBoundary.length() > boundary.length() + 6) {
                request.stringUnparsed = request.lastBoundary.substr(0, request.lastBoundary.length() - boundary.length() - 6);
                request.lastBoundary = request.lastBoundary.substr(request.lastBoundary.length() - boundary.length() - 6);
            } else
                request.stringUnparsed = "";
        } else {
            request.stringUnparsed.insert(0, request.lastBoundary);
            request.lastBoundary = request.stringUnparsed.substr(request.stringUnparsed.length() - boundary.length() - 6);
            request.stringUnparsed.resize(request.stringUnparsed.length() - boundary.length() - 6);
        }
        writeOnFile(request.fileName, request.stringUnparsed);
        request.stringUnparsed = "";
        if (request.binaryRead == request.realContentLength) {
            if (request.lastBoundary == "\r\n"+boundary+"--\r\n") {
                request.done = true;
                request.response = responseBuilder()
                    .addStatusLine("201")
                    .addContentType("text/html")
                    .addLocationFile(request.fileName)
                    .addResponseBody(request.getPageStatus(201));
                throw "201";
            }
            std::remove(request.fileName.c_str());
            request.response = responseBuilder()
                .addStatusLine("400")
                .addContentType("text/html")
                .addResponseBody(request.getPageStatus(400));
            throw "400";
        }
    } catch (std::exception &e) {
        std::remove(request.fileName.c_str());
        request.response = responseBuilder()
            .addStatusLine("400")
            .addContentType("text/html")
            .addResponseBody(request.getPageStatus(400));
        throw "400";
    }
}

void requestChunked(Request &request) {
    std::string tmpBody;
    std::string &s = request.stringUnparsed;
    std::string &chnkUnparsed = request.chunkedUnparsed;
    for (size_t i = 0; i < s.length();i++) {
        if (request.chunkSize == -1 && s[i] != '\r')
            chnkUnparsed += s[i];
        else if (request.chunkSize == -1 && s[i] == '\r') {
            request.chunkSize = hexToDec(chnkUnparsed, request) + 3;
            chnkUnparsed = "";
        } else if (request.chunkSize != -1) {
            if (request.chunkSize == 0) {
                if ((chnkUnparsed.length() >= 3 && chnkUnparsed != "\n\r\n") || (chnkUnparsed == "\n\r\n" && request.binaryRead != request.realContentLength)) {
                    request.response = responseBuilder()
                        .addStatusLine("400")
                        .addContentType("text/html")
                        .addResponseBody(request.getPageStatus(400));
                    throw "400";
                }
                chnkUnparsed += s[i];
                continue;
            }
            long long cnt  = 0;
            for (; cnt < request.chunkSize && i < s.length(); cnt++, i++)
                chnkUnparsed += s[i];
            if (cnt) i--;
            request.chunkSize -= cnt;
            if (request.chunkSize == 0) {
                if (chnkUnparsed[0] != '\n' || chnkUnparsed[chnkUnparsed.length() - 1] != '\n' || chnkUnparsed[chnkUnparsed.length() - 2] != '\r'){
                    request.response = responseBuilder()
                        .addStatusLine("400")
                        .addContentType("text/html")
                        .addResponseBody(request.getPageStatus(400));
                    throw "400";
                }
                request.chunkSize = -1;
                for(size_t j = 1; j < chnkUnparsed.length() - 2; j++)
                    tmpBody += chnkUnparsed[j];
                chnkUnparsed = "";
            } else if (request.chunkSize < 0) {
                request.response = responseBuilder()
                    .addStatusLine("400")
                    .addContentType("text/html")
                    .addResponseBody(request.getPageStatus(400));
                throw "400";
            }
        }
    }
    s = tmpBody;
}