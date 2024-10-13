#include "../includes/utils.hpp"

std::vector<std::string> splitWithChar(std::string s, char delim) {
	std::vector<std::string> result;
	std::stringstream ss (s);
	std::string item;

	while (getline (ss, item, delim)) {
        if (!item.empty())
		    result.push_back (item);
	}

	return result;
}

bool checkLimitRead(Request &request, size_t bodySize) {
    mapConstIterator MaxBodySize = request.getLocationBlockWillBeUsed().find("client_max_body_size");
    if (MaxBodySize != request.getLocationBlockWillBeUsed().end()) {
        size_t sizeMax = custAtoi(MaxBodySize->second);
        if (bodySize > sizeMax) {
            request.response = responseBuilder()
            .addStatusLine("413")
            .addContentType("text/html")
            .addResponseBody(request.getPageStatus(413));
            throw "413";
        }
    }
    if (request.realContentLength < bodySize) {
        request.response = responseBuilder()
        .addStatusLine("413")
        .addContentType("text/html")
        .addResponseBody(request.getPageStatus(413));
        throw "413";
    }
    return false;
}

std::vector<std::string> customSplitRequest(const std::string& input, const std::string& delimiter) {
    std::vector<std::string> result;
    std::size_t start = 0;
    std::size_t end = input.find(delimiter);
    while (end != std::string::npos) {
        end += delimiter.length();
        result.push_back(input.substr(start, end - start));
        start = end;
        end = input.find(delimiter, start);
    }

    result.push_back(input.substr(start));
    return result;
}

void writeOnFile(const std::string &filename, const std::string &content) {
    if (content.empty())
        return ;
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error: file not open" << std::endl;
        return ;
    }
    file << content;
}

size_t custAtoi(const std::string &s) {
    size_t res = 0;
    for (size_t i = 0; i < s.length(); i++) {
        res = res * 10 + s[i] - '0';
    }
    return res;
}

size_t hexToDec(const std::string &s, Request &request) {
    size_t res = 0;
    for (size_t i = 0; i < s.length(); i++) {
        if (isdigit(s[i]) || (s[i] >= 'a' && s[i] <= 'f'))
            res = res * 16 + (s[i] >= '0' && s[i] <= '9' ? s[i] - '0' : s[i] - 'a' + 10);
        else {
            request.response = responseBuilder()
                .addStatusLine("400")
                .addContentType("text/html")
                .addResponseBody(request.getPageStatus(400));
            throw "400";
        }
    }
    return res;
}

std::string ftToString(size_t n) {
    std::string s;
    std::stringstream out;
    out << n;
    s = out.str();
    return s;
}

char hexToCharacters(const std::string& hex) {
    std::stringstream ss;
    ss << std::hex << hex;
    unsigned int c;
    ss >> c;
    return static_cast<char>(c);
}

std::string decodeUrl(const std::string &srcString) {
    std::string result;
    size_t length = srcString.size();

    for (size_t i = 0; i < length; ++i) {
        if (srcString[i] == '%' && i + 2 < length) {
            std::string hex = srcString.substr(i + 1, 2);
            result += hexToCharacters(hex);
            i += 2;
        } else if (srcString[i] == '+') {
            result += ' ';
        } else {
            result += srcString[i];
        }
    }
    return result;
}

std::vector<std::string> splitString(const std::string& input, const std::string& delimiter) {
    std::vector<std::string> result;
    std::size_t start = 0;
    std::size_t end = input.find(delimiter);

    while (end != std::string::npos) {
        result.push_back(input.substr(start, end - start));
        start = end + delimiter.length();
        end = input.find(delimiter, start);
    }

    result.push_back(input.substr(start));

    return result;
}

std::string lower(std::string str) {
    std::string ret = "";
    for (std::string::iterator it = str.begin(); it != str.end(); it++) {
        ret += std::tolower(*it);
    }
    return ret;
}
