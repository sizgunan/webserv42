#pragma once

#include "configFile.hpp"
#include "Server.hpp"
#include "responseBuilder.hpp"
#include <sstream>

typedef std::pair<std::string, std::string> pair;

class Request {
public:
    bool done;
    std::streampos lastPos;
    std::string stringUnparsed;
    std::string chunkedUnparsed;
    char buffer[91337];
    std::string fileName;
    std::string lastBoundary;
    std::string firstPart;
    size_t binaryRead;
    long long chunkSize;
    bool checkTimeout;
    char bufferFile[1024];
    int FD;
    
private:
    std::map<std::string, std::string> directives;
    std::map<std::string, std::string> locationBlockWillBeUsed;
    std::map<std::string, std::map<int, std::string> > pages;


    std::vector<std::map<std::string, std::string> > locationsBlock;

    std::string httpVerb;
    std::string uri;
    std::string httpVersion;
    std::string boundary;

    std::map<std::string, std::string> httpRequestHeaders;

    std::string requestHeader;
    std::string requestBody;
    std::string queryString;

    bool requestBodyChunk;

    //test
    std::string requestOutputTest;

    //Response
    std::map<std::string, std::string> urlencodedResponse;

    bool saveLastBS;

    time_t timeout;


public:

    Request();

    responseBuilder response;
    size_t realContentLength;
    size_t reachedBodyLength;
    std::string extension;

    std::string RePort;
    std::string ReHost;

    bool dup;
	std::string result;
    bool reCheck;
    //* GETTERS
    time_t getTimeout(void) const;
    const std::map<std::string, std::map<int, std::string> > &getPages() const;
    const std::map<std::string, std::string> &getDirectives() const;
    const std::vector<std::map<std::string, std::string> > &getLocationsBlock() const;

    const std::string &getHttpVerb(void) const;
    const std::string &getUri(void) const;
    const std::string &getHTTPVersion(void) const;
    const std::string &getBoundary(void) const;


    const std::string &getRequestBody() const ;

    std::map<std::string, std::string> &getHttpRequestHeaders();
    const std::map<std::string, std::string> &getLocationBlockWillBeUsed() const ;
    const std::string getPageStatus(int status) const ;

    bool getRequestBodyChunk(void);

   const std::string &getQueryString(void) const;

    //! Responses


    bool getSaveLastBS(void) const;

    //* SETTERS

    void setTimeout();
    void setRequestBody(std::string &setter);
    void clearRequestBody();


    void setHttpRequestHeaders(pair setPair);

    void setHttpVerb(std::string &setter);
    void setUri(std::string &setter);
    void setHTTPVersion(std::string &setter);
    void setBoundary(std::string &setter);

    void setDirectivesAndPages(std::map<std::string, std::string> directives, std::map<std::string, std::map<int, std::string> > Pages);
    void setLocationsBlock(std::vector<std::map<std::string, std::string> > other);

    void setLocationBlockWillBeUsed(std::map<std::string, std::string> &other);
    void setRequestBodyChunk(bool chunk) ;

    void setQueryString(std::string &setter);
    void setUrlencodedResponse(std::map<std::string, std::string> &setter);

    void setSaveLastBS(bool chunk) ;


    ~Request() ;

};

void parseUri( Request &request, std::string &uri);
void fillFirstPartOfMultipart(Request &request);
void multipartBody(Request &request);
void requestChunked(Request &request);
bool parseFirstLine(std::string &s, Request &request);
bool parseDefaultLine(std::string &s, Request &request);