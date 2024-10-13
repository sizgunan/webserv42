#pragma once

#include <string>

enum STATUS_CODE_ENUM {
    OK ,                        //* 200
    CREATED ,                   //* 201
    NO_CONTENT ,                //* 204
    MOVED_PERMANETLY ,          //* 301
    BAD_REQUEST ,               //* 400
    FORBIDDEN ,                 //* 403
    NOT_FOUND ,                 //* 404
    CONFLICT ,                  //* 409
    REQUEST_TOO_LARGE ,         //* 413
    URI_TOO_LONG ,              //* 414
    INTERNAL_SERVER_ERROR ,     //* 500
    NOT_IMPLEMENTED,            //* 501
    METHOD_NOT_ALLOWED,         //* 405
    BAD_GATEWAY,                //* 502
    FOUND,                      //* 302
    TIMEOUT                     //* 408
};


class responseBuilder {
    std::string res;
public:

    std::multimap<std::string, std::string> headersResponses;
    std::string resultMsg;
    std::string body;

    void defineStatusLine(const std::string &type);
    void defineContentType(const std::string &extension);

    responseBuilder& addStatusLine(const std::string &type);
    responseBuilder& addLocation(std::string location);
    responseBuilder& addContentType(const std::string &extension);
    responseBuilder& addContentLength();
    responseBuilder& addContentLength(const std::string &content);
    responseBuilder& addResponseBody(const std::string &responseBody);
    responseBuilder& addLocationFile(const std::string &location);
    responseBuilder& addCustomHeader(const std::string &header, const std::string &value);

    std::string &build();
};
