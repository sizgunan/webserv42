#include "../includes/webserve.hpp"
#include <fstream>

Request::Request() :
    done(false),
    lastPos(-1),
    binaryRead(0),
    chunkSize(-1),
    checkTimeout(false),
    FD(-1),
    httpVerb (""),
    uri(""),
    httpVersion(""),
    boundary(""),
    requestHeader(""),
    requestBody(""),
    requestBodyChunk(false),
    requestOutputTest("20000000000000000000"),
    saveLastBS(false),
    timeout(time(0)),
    realContentLength(0),
    reachedBodyLength(0),
    dup(false),
    reCheck(false)
    {}

//* GETTERS


const std::map<std::string, std::string> &Request::getDirectives() const {
    return (this->directives);
}
const std::map<std::string, std::map<int, std::string> > &Request::getPages() const {
    return (this->pages);
}
const std::vector<std::map<std::string, std::string> > &Request::getLocationsBlock() const {
    return (this->locationsBlock);
}

const std::string &Request::getHttpVerb(void) const {
    return ( this->httpVerb );
}

const std::string &Request::getUri(void) const {
    return ( this->uri );
}

const std::string &Request::getHTTPVersion(void) const {
    return (this->httpVersion );
}

const std::string &Request::getRequestBody() const  {
    return (this->requestBody);
}

std::map<std::string, std::string> &Request::getHttpRequestHeaders() {
    return (httpRequestHeaders);
}

const std::map<std::string, std::string> &Request::getLocationBlockWillBeUsed() const {
    return (this->locationBlockWillBeUsed);
}


bool Request::getRequestBodyChunk(void) {
    return (this->requestBodyChunk);
}

const std::string &Request::getBoundary(void) const {
    return (this->boundary);
}



bool Request::getSaveLastBS(void) const {
    return (this->saveLastBS);
}




const std::string &Request::getQueryString() const {
    return (this->queryString);
}

void Request::setTimeout() {
    this->timeout = time(0);
}

time_t Request::getTimeout() const {
    return this->timeout;
}
void Request::setBoundary(std::string &setter) {
    this->boundary = setter;
}

void Request::setRequestBody(std::string &setter) {
    requestBody += setter;
}



void Request::setDirectivesAndPages(std::map<std::string, std::string> directives, std::map<std::string, std::map<int, std::string> > pages) {
    this->directives = directives;
    this->pages = pages;
}

void Request::setLocationsBlock(std::vector<std::map<std::string, std::string> > other) {
    this->locationsBlock = other;
}


void Request::setHttpVerb(std::string &setter) {
    this->httpVerb = setter;
}

void Request::setUri(std::string &setter) {
    this->uri = setter;
}

void Request::setHTTPVersion(std::string &setter) {
    this->httpVersion = setter;
}


void Request::setLocationBlockWillBeUsed(std::map<std::string, std::string> &other) {
    this->locationBlockWillBeUsed = other;
}

void Request::setRequestBodyChunk(bool chunk) {
    this->requestBodyChunk = chunk;
}

void Request::setHttpRequestHeaders(pair setPair) {

    httpRequestHeaders[setPair.first] = setPair.second;
}


void Request::setUrlencodedResponse(std::map<std::string, std::string> &setter) {
    this->urlencodedResponse = setter;
}


void Request::setSaveLastBS(bool chunk) {
    this->saveLastBS = chunk;
}


void Request::setQueryString(std::string &setter) {
    this->queryString = setter;
}

Request::~Request() {}


const std::string Request::getPageStatus(int status) const {
    if (!getLocationBlockWillBeUsed().count("location")) {
        return (getPages().at(".").at(status));
    }
    std::string LocationName = getLocationBlockWillBeUsed().at("location");
    std::string page = getPages().at(LocationName).at(status);
    return (page);
}


void Request::clearRequestBody() {
    this->requestBody = "";
}