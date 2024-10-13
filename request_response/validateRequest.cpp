#include "../includes/webserve.hpp"
static void handleUploadingError(Request &request, const char *statusCode);


std::vector<std::string> splitUri(const std::string& input, const std::string& delimiter) {

    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end = 0;

    while ((end = input.find(delimiter, start)) != std::string::npos) {
        tokens.push_back(input.substr(start, end - start + delimiter.length()));
        start = end + delimiter.length();
    }
    tokens.push_back(input.substr(start));

    return tokens;
}


static std::string fetchTheExactDirectory(const std::string uri) {

    DIR *dir_ptr; dir_ptr = opendir(uri.c_str());
    if (dir_ptr != NULL) {
        if (closedir(dir_ptr) == -1) {
            std::cerr << "Error: cannot close the directory" << std::endl;
            throw "Error: closedir()";
        }
        return (uri);
    }
    std::vector<std::string> vectorDirectory = splitUri(uri, "/");
    if ( vectorDirectory.size() == 1 ) {
        return (uri);
    }

    std::string concatenateDirectories;
    for (const_vector_it it = vectorDirectory.begin(); it != vectorDirectory.end(); ++it) {
        std::string directory = (*it);
        if (directory[directory.length() - 1] == '/')
            concatenateDirectories += directory;
    }

    concatenateDirectories.erase(concatenateDirectories.end() - 1);
    return (concatenateDirectories);
}

static std::map<std::string, std::string> fetchSuitableLocationBlock(Request &request, std::string uri)
{
    std::vector<std::map<std::string, std::string> > locationsBlock = request.getLocationsBlock();
    std::map<std::string, std::string> found ;

    for (vectorToMapIterator it = locationsBlock.begin(); it != locationsBlock.end(); it++) {

        std::map<std::string, std::string> mapIterator = (*it);
        std::string location_match = mapIterator["location"];

        if ( location_match == uri ) {
           return (mapIterator);
        }

    }

    for (vectorToMapIterator it = locationsBlock.begin(); it != locationsBlock.end(); it++) {

        std::map<std::string, std::string> mapIterator = (*it);
        std::string location_match = mapIterator["location"];

        if ( location_match == uri ) {
           return (mapIterator);
        }
    }

    std::string directoryUri = fetchTheExactDirectory(uri);

    while (!directoryUri.empty()) {
        for (std::vector<std::map<std::string, std::string> >::iterator it = locationsBlock.begin(); it != locationsBlock.end(); it++) {
            if (it->at("location") == directoryUri) {
                return *it;
            }
        }
        int ind = directoryUri.find_last_of('/');
        if (ind != -1)
            directoryUri.erase(ind);
        else directoryUri.clear();
    }
    return (found);
}

void validateHeader(Request &request) {
    if (request.getHttpVerb() == "PUT" || request.getHttpVerb() == "OPTIONS"
        || request.getHttpVerb() == "TRACE" || request.getHttpVerb() == "CONNECT"
        || request.getHttpVerb() == "PATCH" || request.getHttpVerb() == "PROPFIND"
        || request.getHttpVerb() == "PROPPATCH" || request.getHttpVerb() == "MKCOL"
        || request.getHttpVerb() == "COPY" || request.getHttpVerb() == "MOVE"
        || request.getHttpVerb() == "LOCK" || request.getHttpVerb() == "UNLOCK"
        || request.getHttpVerb() == "REPORT" || request.getHttpVerb() == "MKACTIVITY"
        || request.getHttpVerb() == "CHECKOUT" || request.getHttpVerb() == "MERGE"
        || request.getHttpVerb() == "M-SEARCH" || request.getHttpVerb() == "NOTIFY"
        || request.getHttpVerb() == "SUBSCRIBE" || request.getHttpVerb() == "UNSUBSCRIBE"
        || request.getHttpVerb() == "PURGE" || request.getHttpVerb() == "LINK"
        || request.getHttpVerb() == "UNLINK" || request.getHttpVerb() == "HEAD") {
        request.response = responseBuilder()
            .addStatusLine("501")
            .addContentType("text/html")
            .addResponseBody(request.getPageStatus(501));
        throw "501";
    }
    
    if (request.getHttpVerb() != "POST" && request.getHttpVerb() != "GET" && request.getHttpVerb() != "DELETE") {
        request.response = responseBuilder()
            .addStatusLine("405")
            .addContentType("text/html")
            .addResponseBody(request.getPageStatus(405));
        throw "405";
    }
    std::map<std::string, std::string> location = fetchSuitableLocationBlock(request, request.getUri());

    if (!location.empty()) {
        request.setLocationBlockWillBeUsed(location) ;
    } else if ( request.getLocationBlockWillBeUsed().empty() ) {
        std::map<std::string, std::string> defaultLocation = request.getDirectives();
        request.setLocationBlockWillBeUsed(defaultLocation);
        location = request.getLocationBlockWillBeUsed();
    }
    if (location.find("allowedMethods") != location.end()) {
        std::string input = location["allowedMethods"];
        std::vector<std::string> theAllowedMethods = splitString(input, " ");
        const_vector_it itAllowedMethods = std::find(theAllowedMethods.begin(), theAllowedMethods.end(), request.getHttpVerb());

        if (itAllowedMethods == theAllowedMethods.end()) {
            request.response = responseBuilder()
            .addStatusLine("405")
            .addContentType("text/html")
            .addResponseBody(request.getPageStatus(405));
            throw "405";
        }
    }
    if (request.getUri().length() > 2048 ) {
        request.response = responseBuilder()
            .addStatusLine("414")
            .addContentType("text/html")
            .addResponseBody(request.getPageStatus(414));
        throw "414" ;
    }
    std::map<std::string, std::string> httpRequestHeaders = request.getHttpRequestHeaders();
    std::string transferEncoding = httpRequestHeaders["Transfer-Encoding"];

    if (!transferEncoding.empty() && transferEncoding != "chunked") {
        request.response = responseBuilder()
            .addStatusLine("501")
            .addContentType("text/html")
            .addResponseBody(request.getPageStatus(501));
        throw "501" ;
    }

    if (location.find("return") != location.end() ) {

		std::vector<std::string> v = splitString(location["return"], " ");
		std::string changeLocation = v[1], statusCode = v[0];
        if (changeLocation.find("://") == std::string::npos)
            changeLocation = "http://" + changeLocation;

        request.response = responseBuilder()
            .addStatusLine(statusCode)
            .addLocationFile(changeLocation)
            .addContentType("text/html")
            .addResponseBody("<html><h1>" + statusCode + "</h1></html>");

        throw "return directive";
    }
    if (!httpRequestHeaders.count("Content-Type") || httpRequestHeaders["Content-Type"].find("multipart/form-data;") == std::string::npos) return ;
    std::string contentType = httpRequestHeaders["Content-Type"];
    size_t pos = contentType.find("multipart/form-data; boundary=");
    if (pos != std::string::npos) {
        std::string boun = contentType.substr(pos + 30);
        boun.insert(0, "--");
        request.setBoundary(boun);
        request.getHttpRequestHeaders().erase("Content-Type");
        request.getHttpRequestHeaders().insert(std::make_pair("Content-Type", "multipart/form-data;"));
    } else {
        request.response = responseBuilder()
            .addStatusLine("400")
            .addContentType("text/html")
            .addResponseBody(request.getPageStatus(400));
        throw "400";
    }

    if (!location.count("upload_enable") || location["upload_enable"] != "on" || !location.count("upload_store"))
        handleUploadingError(request, "403");
    
    DIR *dir_ptr = opendir(location["upload_store"].c_str());
    if (dir_ptr == NULL)
        handleUploadingError(request, "500");

    if (closedir(dir_ptr) == -1) {
        std::cerr << "Error: cannot close the directory" << std::endl;
        throw "Error: closedir()";
    }
}


static void handleUploadingError(Request &request, const char * statusCode) {

	int status = std::atoi(statusCode);

	std::pair<std::string, std::string> p = std::make_pair("Connection", "close");
	request.setHttpRequestHeaders(p);
    
	request.response = responseBuilder()
		.addStatusLine(statusCode)
		.addContentType("text/html")
		.addResponseBody(request.getPageStatus(status));
	throw statusCode;
}
