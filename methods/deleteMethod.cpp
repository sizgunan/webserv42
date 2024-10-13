#include "../includes/webserve.hpp"

static void errorOccurredOnDeleteion(Request &request, std::string path) {

    if (access(path.c_str(), W_OK) == 0) {
        std::string page = request.getPageStatus(500);
        request.response = responseBuilder()
            .addStatusLine("500")
            .addContentType("text/html")
            .addResponseBody(page);
        throw "500";
    } else {
        std::string page = request.getPageStatus(403);
        request.response = responseBuilder()
            .addStatusLine("403")
            .addContentType("text/html")
            .addResponseBody(page);
        throw "403";
    }

}


static std::string deleteAllFolderContent(Request &request,std::string &absolutePath, int frame) {

    DIR *dir_ptr;
    struct dirent *read_dir;
    std::string checkFail;

    dir_ptr = opendir(absolutePath.c_str());
    if (!dir_ptr)
        errorOccurredOnDeleteion(request, absolutePath);
    while ((read_dir = readdir(dir_ptr)) != NULL)
    {
        std::string cmp = read_dir->d_name;
        std::string dir = absolutePath + '/' + cmp;
        if (cmp.compare(".") && cmp.compare("..")) {
            if (read_dir->d_type == DT_DIR) {
                checkFail = deleteAllFolderContent(request, dir, frame + 1);
                if ( ! checkFail.empty() ) {
                    closedir(dir_ptr);
                    if (!frame)
                        errorOccurredOnDeleteion(request, checkFail);
                    return (checkFail);
                }
            }
            if ( remove(dir.c_str()) ) {
                closedir(dir_ptr);
                return (dir);
            }
        }
    }
    closedir(dir_ptr);
    return ("");
}

void deleteFolder(std::string &absolutePath, std::string &uri, Request &request) {
    (void) uri;
    std::string response;

    if (!request.getSaveLastBS()) {
        std::string page = request.getPageStatus(409);
        request.response = responseBuilder()
            .addStatusLine("409")
            .addContentType("text/html")
            .addResponseBody(page);
        throw "409";
    }

    std::string check = deleteAllFolderContent(request, absolutePath, 0);
    if (check.empty()) {
        if (rmdir(absolutePath.c_str()) != 0) {
            std::string page = request.getPageStatus(502);
            request.response = responseBuilder()
                .addStatusLine("502")
                .addContentType("text/html")
                .addResponseBody(page);
            throw "Error(): rmdir system call";
        } else {
            std::string page = request.getPageStatus(204);
            request.response = responseBuilder()
                .addStatusLine("204")
                .addContentType("text/html")
                .addResponseBody(page);
            throw "204";
        }
    }
}

void deleteFile(std::string &absolutePath, std::string &uri, Request &request) {
    (void) uri;
    int out = std::remove(absolutePath.c_str());

    if (out != 0) {
        std::string page = request.getPageStatus(502);
        request.response = responseBuilder()
            .addStatusLine("502")
            .addContentType("text/html")
            .addResponseBody(page);
        throw "Error(): remove system call";
    } else {
        std::string page = request.getPageStatus(204);
        request.response = responseBuilder()
            .addStatusLine("204")
            .addContentType("text/html")
            .addResponseBody(page);
        throw "204";
    }
}
