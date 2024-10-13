#include "../includes/webserve.hpp"

void    parseUri( Request &request, std::string &uri) {
    if (uri.empty()) return;
    std::vector<std::string> v = splitWithChar(uri, '/');
    bool hasBs = (uri[uri.length() - 1] == '/');
    request.setSaveLastBS(hasBs);
    std::stack<std::string> s;
    for (size_t i = 0; i < v.size(); i++) {
        if (v[i] == "..") {
            if (!s.empty())
                s.pop();
        } else if (v[i] != ".") {
            s.push(v[i]);
        }
    }
    if (s.empty()) {
        uri = "/";
        request.setUri(uri);
        return;
    }
    std::string lst = s.top();
    s.pop();
    size_t pos = lst.rfind('?');
    if (pos != std::string::npos) {
        std::string queryString = lst.substr(pos + 1);
        request.setQueryString(queryString);
        lst = lst.substr(0, pos);
    }
    if (lst.empty()) request.setSaveLastBS(true);
    else s.push(lst);
    uri = "";
    while (!s.empty()) {
        uri = "/" + s.top() + uri;
        s.pop();
    }
    if (uri.empty()) uri = "/";
    request.setUri(uri);
}

bool parseFirstLine(std::string &s, Request &request) {
    std::vector<std::string> lines = splitWhiteSpaces(s);
    if (lines.size() != 3 || lines[2] != "HTTP/1.1" || lines[1][0] != '/') {
        return false;
    }
    for (size_t i = 0; i < lines[0].length(); i++) {
        if (!isalpha(lines[0][i]) && !isupper(lines[0][i]))
            return false;
    }
    parseUri(request, lines[1]);
    request.setHttpVerb(lines[0]);
    request.setHTTPVersion(lines[2]);
    return true;
}

bool parseDefaultLine(std::string &s, Request &request) {
    std::vector<std::string> v = splitWithChar(s, ':');
    if (v.size() < 2)
        return false;
    if (v[0][v[0].length() - 1] == ' ')
        return false;
    int l = -1;
    while (v[1][++l] == ' ');
    v[1].erase(0, l);
    if ((v[0] == "Content-Length" || v[0] == "Host" || v[0] == "Transfer-Encoding") && request.getHttpRequestHeaders().count(v[0]))
        return false;
    std::string value = "";
    for (size_t i = 1; i < v.size(); i++) {
        value += v[i];
        if (i < (s[s.length()-1] == ':' ? v.size() : v.size() - 1))
            value += ":";
    }
    request.setHttpRequestHeaders(make_pair(v[0], value.erase(value.length() - 2)));
    return true;
}
