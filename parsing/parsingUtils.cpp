/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsingUtils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-mal <aait-mal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 15:30:31 by obelaizi          #+#    #+#             */
/*   Updated: 2024/03/13 02:47:31 by aait-mal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserve.hpp"

bool StringIsNum(std::string &s) {
    for (size_t i =0 ; i< s.size();i++) if (!isdigit(s[i])) return (false);
    if (s.size() > 3 || s.size() == 0) return (false);
    return (true);
}

void fillTheMap(std::map<int, std::string> &map, std::string &Err) {
    std::vector<std::string> v = splitWithChar(Err, '\n');
    int num;
    for (std::vector<std::string>::iterator line = v.begin(); line != v.end(); line++) {
        std::vector<std::string> v2 = splitWithChar(*line, ' ');
        if (v2.size() != 2)
            throw std::runtime_error("Error: error_page has invalid format");
        StringIsNum(v2[0]) == false ? throw std::runtime_error("Error: invalid error code") : num = atoi(v2[0].c_str());
        if (num < 400 || num > 599)
            throw std::runtime_error("Error: error code should be between 400 and 599");
        std::ifstream file(v2[1].c_str());
        if (!file) continue;
        std::string s = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if (s.size() > 1e6) continue;
        map[num] = s;
    }
   int statusCodes[] = {200, 201, 204, 301, 400, 403, 404, 405, 408, 409, 413, 414, 500, 501, 502, 418};
    for (int i = 0; i < 16; i++) {
        std::stringstream codeStr;
        codeStr << statusCodes[i];
        if (map.count(statusCodes[i]) == 0) {
            std::ifstream file(("./pages/response_pages/" + codeStr.str() + ".html").c_str());
            if (!file) throw std::runtime_error("Error: missing error page " + codeStr.str() + ".html");
            std::string s = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            if (s.size() > 1e6) throw std::runtime_error("Error: error page " + codeStr.str() + ".html is too big");
            map[statusCodes[i]] = s;
        }
    }
}

void Server::fillErrorPages(Server &s) {
    std::string ErrorPages;
    fillTheMap(s.pages["."], ErrorPages);
    for (size_t i = 0; i < s.locationsBlock.size(); i++) {
        std::map<std::string, std::string> &location = s.locationsBlock[i];
        ErrorPages = location["error_page"];
        try {
            fillTheMap(s.pages[location["location"]], ErrorPages);
            ErrorPages.clear();
        } catch (std::runtime_error &e) {
            std::cerr << e.what() << std::endl;
            exit(1);
        }
    }
}

