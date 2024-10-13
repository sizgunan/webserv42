#include "../includes/webserve.hpp"

std::string defineMimeType(const std::string &type) {

    if (type == "css")
        return "text/css";
    if (type == "xml")
        return "text/xml";
    if (type == "jpg")
        return "image/jpeg";
    if (type == "jpeg")
        return "image/jpeg";
    if (type == "png")
        return "image/png";
    if (type == "webp")
        return "image/webp";
    if (type == "js")
        return "application/javascript";
    if (type == "zip")
        return "application/zip";
    if (type == "img")
        return "application/octet-stream";
    if (type == "mp3")
        return "audio/mpeg";
    if (type == "mp4")
        return "video/mp4";
    if (type == "webm")
        return "video/webm";
    if (type == "json")
        return "application/json";
    if (type == "pdf")
        return "application/pdf";
    if (type == "txt")
        return "text/plain";
    if (type == "html")
        return ("text/html");
    return "application/octet-stream";
}

void responseBuilder::defineStatusLine(const std::string &type) {

    std::string ret;
    std::stringstream ss;
    ss << type;
    int typeCode;
    ss >> typeCode;

    switch (typeCode) {
        case 200 : ret = RESPONSE_OK;  break ;
        case 201 : ret = RESPONSE_CREATED;  break ;
        case 204 : ret = RESPONSE_NO_CONTENT;  break ;
        case 301 : ret = RESPONSE_MOVED_PERMANETLY;  break ;
        case 302 : ret = RESPONSE_FOUND;  break ;
        case 400 : ret = RESPONSE_BAD_REQUEST;  break ;
        case 403 : ret = RESPONSE_FORBIDDEN;  break ;
        case 404 : ret = RESPONSE_NOT_FOUND;  break ;
        case 405 : ret = RESPONSE_METHOD_NOT_ALLOWED;  break ;
        case 408 : ret = RESPONSE_REQUEST_TIMEOUT;  break ;
        case 409 : ret = RESPONSE_CONFLICT;  break ;
        case 413 : ret = RESPONSE_REQUEST_TOO_LARGE;  break ;
        case 414 : ret = RESPONSE_URI_TOO_LONG;  break ;
        case 500 : ret = RESPONSE_INTERNAL_SERVER_ERROR;  break ;
        case 501 : ret = RESPONSE_NOT_IMPLEMENTED;  break ;
        case 502 : ret = RESPONSE_BAD_GATEWAY;  break ;
        default : ret = RESPONSE_INTERNAL_SERVER_ERROR;  break ;
    }
    resultMsg = ret;
}

void responseBuilder::defineContentType(const std::string &extension) {

    if (extension == "text/html"){
        headersResponses.insert(std::make_pair(CONTENT_TYPE, extension));
        return ;
    }
    std::string type;

    size_t lastSlashPos = extension.find_last_of('.');
    std::string version = extension.substr(lastSlashPos + 1);

    type = defineMimeType(version);
    headersResponses.insert(std::make_pair(CONTENT_TYPE, type));
}


responseBuilder& responseBuilder::addStatusLine(const std::string &type) {
    defineStatusLine(type);
    return (*this);
}

responseBuilder& responseBuilder::addContentType(const std::string &extension) {
    if (headersResponses.find(CONTENT_TYPE) == headersResponses.end())
        defineContentType(extension);
    return (*this);
}

responseBuilder& responseBuilder::addLocation(std::string location) {

    location += "/";
    headersResponses.insert(std::make_pair(LOCATION, location));
    return (*this);
}

responseBuilder& responseBuilder::addContentLength() {

    size_t length = body.length();
    std::stringstream ss;

    if (length) length += 2;
    ss << length;
    if (headersResponses.find(CONTENT_LENGTH) == headersResponses.end())
        headersResponses.insert(std::make_pair(CONTENT_LENGTH, ss.str()));
    return (*this);
}

responseBuilder& responseBuilder::addCustomHeader(const std::string &header, const std::string &value) {
    headersResponses.insert(std::make_pair(header+":", value));
    return (*this);
}

responseBuilder& responseBuilder::addLocationFile(const std::string &location) {

    headersResponses.insert(std::make_pair(LOCATION, location));
    return (*this);
}

responseBuilder& responseBuilder::addContentLength(const std::string &content) {

    size_t number = content.size();
    std::ostringstream oss ;

    if (number) number += 2;
    
    oss << number ;
    if (headersResponses.find(CONTENT_LENGTH) == headersResponses.end())
        headersResponses.insert(std::make_pair(CONTENT_LENGTH, oss.str()));
    return (*this);
}

responseBuilder& responseBuilder::addResponseBody(const std::string &responseBody) {

    if (this->body.length() == 0) {
        this->body = responseBody;
        this->addContentLength();
    }
    return (*this);

}

std::string &responseBuilder::build() {

    res = "HTTP/1.1 " + resultMsg + CRLF;

    for (std::multimap<std::string, std::string>::iterator it = headersResponses.begin(); it !=  headersResponses.end(); it++)
        res += it->first + it->second + CRLF;
    
    res += "Keep-Alive: timeout=5\r\n\r\n";
    if (body.length() != 0)
        res.insert(res.length(), body + CRLF);
    return res;
}
