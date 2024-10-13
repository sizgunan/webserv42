#include "../includes/cgi.hpp"

void redirectStdoutStderr(Pipe& pipe) {
    dup2(pipe.getWriteEnd(), STDOUT_FILENO);
    dup2(pipe.getWriteEnd(), STDERR_FILENO);
}

std::string readFromPipeAndClose(int readEnd) {
    std::ostringstream ss;
    std::vector<char> buffer(4096);
    ssize_t bytesRead;

    while ((bytesRead = read(readEnd, &buffer[0], buffer.size())) > 0) {
        ss.write(&buffer[0], bytesRead);
    }

    close(readEnd);

    if (bytesRead == -1) {
        std::cerr << "Error reading from pipe" << "\n";
        throw "REMOVE_THE_CLIENT";
    }

    return ss.str();
}

int executeChildProcess(const std::string& interpreter, const std::string& scriptFilePath,
    std::map<std::string, std::string>& envVars) {
    std::vector<char*> argv;
    std::vector<char*> envp;

    argv.push_back(const_cast<char*>(interpreter.c_str()));
    argv.push_back(const_cast<char*>(scriptFilePath.c_str()));
    argv.push_back(NULL);

    for (std::map<std::string, std::string>::iterator it = envVars.begin(); it != envVars.end(); ++it) {
        std::string envVar = it->first + "=" + it->second;
        envp.push_back(strdup(envVar.c_str()));
    }

    envp.push_back(NULL);

    execve(interpreter.c_str(), &argv[0], &envp[0]);

    // Free the duplicated strings after execve
    for (size_t i = 0; i < envp.size(); ++i) {
        free(envp[i]);
    }

    // If execve fails
    std::cerr << "Error executing interpreter" << "\n";
    exit(EXIT_FAILURE);
}

std::vector<std::string> splitWithString(const std::string& s, const std::string& delimiter) {
    std::vector<std::string> result;
    std::size_t start = 0;
    std::size_t found = s.find(delimiter);

    while (found != std::string::npos) {
        result.push_back(s.substr(start, found - start));
        start = found + delimiter.length();
        found = s.find(delimiter, start);
    }

    result.push_back(s.substr(start));

    return result;
}
