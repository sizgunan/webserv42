#include "../includes/cgi.hpp"

std::pair<std::string, std::string> handleCgiGet(const std::string& file, const std::string& interpreterPath, Request &request) {

    try {
        std::string response;
        Pipe pipe;
        pid_t pid;

        std::map<std::string, std::string> headersMap = request.getHttpRequestHeaders();
        headersMap["REQUEST_METHOD"] = request.getHttpVerb();
        headersMap["REQUEST_URI"] = request.getUri();
        headersMap["SERVER_PROTOCOL"] = request.getHTTPVersion();

        std::map<std::string, std::string> parsedHeaders = parseHeaders(headersMap);
        std::map<std::string, std::string> envVars;

        pid = fork();

        if (pid == -1) {
            std::cerr << "Error forking process.\n";
            return std::make_pair(std::string(), std::string());
        } else if (pid == 0) {
            close(pipe.getReadEnd());
            redirectStdoutStderr(pipe);

            envVars = fillEnv(parsedHeaders);
            envVars.erase("CONTENT_LENGTH");
            envVars.erase("CONTENT_TYPE");

            envVars["SCRIPT_NAME"] = file;
            envVars["SCRIPT_FILENAME"] = file;
            if (!request.getQueryString().empty())
                envVars["QUERY_STRING"] = request.getQueryString();

            signal(SIGALRM, handleTimeout);
            alarm(10);
            executeChildProcess(interpreterPath, file, envVars);
            alarm(0);
        } else {
            close(pipe.getWriteEnd());
            response = readFromPipeAndClose(pipe.getReadEnd());
            
            int status;
            waitpid(pid, &status, 0);

            if (WIFEXITED(status)) {
                int exitStatus = WEXITSTATUS(status);

                if (exitStatus != 0) {
                    std::cerr << "CGI Warning!! Child process exited with status: " << exitStatus << "\n";
                    return std::make_pair("Content-Type: text/html\r\n", request.getPageStatus(500));
                }
            } else if (WIFSIGNALED(status)) {
                int signalNumber = WTERMSIG(status);

                std::cerr << "CGI Warning!! Child process terminated by signal: " << signalNumber << "\n";
                if (signalNumber == SIGALRM)
                    return std::make_pair("Content-Type: text/html\r\n", request.getPageStatus(408));
                return std::make_pair("Content-Type: text/html\r\n", request.getPageStatus(500));
            } else {
                std::cerr << "CGI Error!! Child process terminated abnormally.\n";
                return std::make_pair("Content-Type: text/html\r\n", request.getPageStatus(500));
            }
        }

        return splitHeadersAndBody(response);
    } catch (const std::exception& e) {
        std::cerr << "CGI Exception: " << e.what() << "\n";
        return std::make_pair("Content-Type: text/html\r\n", request.getPageStatus(500));
    }
}

std::pair<std::string, std::string> handleCgiPost(const std::string& file, const std::string& interpreterPath, Request &request) {

    try {
        std::string response;
        Pipe pipe;
        pid_t pid;

        std::map<std::string, std::string> headersMap = request.getHttpRequestHeaders();
        headersMap["REQUEST_METHOD"] = request.getHttpVerb();
        headersMap["REQUEST_URI"] = request.getUri();
        headersMap["SERVER_PROTOCOL"] = request.getHTTPVersion();
        
        std::map<std::string, std::string> parsedHeaders = parseHeaders(headersMap);
        std::map<std::string, std::string> envVars;

        const std::string &postData = request.getRequestBody();
        std::string fileName = "/tmp/tmpFile";
        while (std::ifstream(fileName.c_str()))
            fileName += "_";
        std::ofstream tmpFile(fileName);
        tmpFile << postData;
        tmpFile.close();

        pid = fork();

        if (pid == -1) {
            std::cerr << "Error forking process.\n";
            return std::make_pair(std::string(), std::string());
        } else if (pid == 0) {
            close(pipe.getReadEnd());
            redirectStdoutStderr(pipe);
            int tmpFileDescriptor = open(fileName.c_str(), O_RDONLY);
            if (tmpFileDescriptor < 0) {
                std::cerr << "Error opening tmpFile for reading.\n";
                exit(EXIT_FAILURE);
            }
            if (dup2(tmpFileDescriptor, STDIN_FILENO) == -1) {
                std::cerr << "Error redirecting standard input.\n";
                close(tmpFileDescriptor);
                exit(EXIT_FAILURE);
            }

            close(tmpFileDescriptor);
            std::remove(fileName.c_str());

            envVars = fillEnv(parsedHeaders);
            envVars["SCRIPT_NAME"] = file;
            envVars["SCRIPT_FILENAME"] = file;
            if (!request.getQueryString().empty())
                envVars["QUERY_STRING"] = request.getQueryString();

            signal(SIGALRM, handleTimeout);
            alarm(10);
            executeChildProcess(interpreterPath, file, envVars);
            alarm(0);
        } else {
            close(pipe.getWriteEnd());
            response = readFromPipeAndClose(pipe.getReadEnd());

            int status;
            waitpid(pid, &status, 0);

            if (WIFEXITED(status)) {
                int exitStatus = WEXITSTATUS(status);

                if (exitStatus != 0) {
                    std::cerr << "CGI Warning!! Child process exited with status: " << exitStatus << "\n";
                    return std::make_pair("Content-Type: text/html\r\n", request.getPageStatus(500));
                }
            } else if (WIFSIGNALED(status)) {
                int signalNumber = WTERMSIG(status);

                std::cerr << "CGI Warning!! Child process terminated by signal: " << signalNumber << "\n";
                if (signalNumber == SIGALRM)
                    return std::make_pair("Content-Type: text/html\r\n", request.getPageStatus(408));
                return std::make_pair("Content-Type: text/html\r\n", request.getPageStatus(500));
            } else {
                std::cerr << "CGI Error!! Child process terminated abnormally.\n";
                return std::make_pair("Content-Type: text/html\r\n", request.getPageStatus(500));
            }
        }

        return splitHeadersAndBody(response);
    } catch (const std::exception& e) {
        std::cerr << "CGI Exception: " << e.what() << "\n";
        return std::make_pair("Content-Type: text/html\r\n", request.getPageStatus(500));
    }
}
