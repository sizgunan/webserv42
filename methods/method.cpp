/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obelaizi <obelaizi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/24 22:00:44 by aait-mal          #+#    #+#             */
/*   Updated: 2024/04/22 16:52:43 by obelaizi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserve.hpp"

void method(Request &request, void (*fileFunc)(std::string &, std::string &, Request &), void (*folderFunc)(std::string &, std::string &, Request &)) {
    
    if (request.getHttpVerb() == "POST")
        uploadRequestBody(request);

    std::string root;
    retrieveRootAndUri(request, root);

    std::string uri = request.getUri();
    uri = decodeUrl(uri);
    request.setUri(uri);

    std::string absolutePath = root + uri;
    if (request.getHttpVerb() == "DELETE") {
        if (absolutePath == root+"/" || absolutePath == root) {
            request.response = responseBuilder()
            .addStatusLine("405")
            .addContentType("text/html")
            .addResponseBody(request.getPageStatus(405));
            throw "405";
        }
    }
    const char *path = absolutePath.c_str();
    struct stat fileStat;

    if (stat(path, &fileStat ) == 0) {
        if (S_ISREG(fileStat.st_mode)) {
            fileFunc(absolutePath, uri, request);
        } else if (S_ISDIR(fileStat.st_mode)) {
            folderFunc(absolutePath, uri, request);
        } else {
            request.response = responseBuilder()
                .addStatusLine("502")
                .addContentType("text/html")
                .addResponseBody(request.getPageStatus(502));
            throw "502";
        }
    } else {
        request.response = responseBuilder()
            .addStatusLine("404")
            .addContentType("text/html")
            .addResponseBody(request.getPageStatus(404));
        throw "404";
    }
}
