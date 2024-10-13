# webserv42
- Table of Contents
    1. Project Overview
    2. Key Components
    3. Configuration File Parsing
    4. I/O Multiplexing
    5. Request Parsing and Response Generation
    6. CGI and Cookie Handling
    7. Building and Running
    8. Resources
    
    ## Project Overview
    
    Webserv42 is an advanced project that involves coding an HTTP server from scratch using C++98. This project aims to deepen understanding of network programming, HTTP protocols, and server architecture.
    
    The subject in english : [en.subject.pdf](https://github.com/user-attachments/files/17356299/en.subject.pdf)

    The subject in french : [fr.subject.pdf](https://github.com/user-attachments/files/17356300/fr.subject.pdf)

    
    ## Key Components
    
    The project was divided into four main parts:
    
    - **Parse of configuration file**: this section covers how the server reads and interprets configuration files to set up its parameters and behavior.
    - **I/O Multiplexing** : We implement the **`select()`** system call to manage multiple simultaneous connections efficiently, allowing the server to handle numerous clients without blocking.
    - **Parse Request [GET, POST, DELETE] / Generating Response** : This part focuses on processing incoming HTTP requests, including various methods such as GET, POST, and DELETE, with generating appropriate responses based on those requests.
    - **CGI (php and py) and Cookie handling** : We explore how to execute scripts in different languages (PHP, Python) through the server and manage cookies for session handling.

![Untitled](https://github.com/user-attachments/assets/04a5a4bf-35d5-4809-b76d-c12e4cb59fea)



# I- Parse of Configuration File :

- Configuration file is a structured text file that defines various settings and parameters for the server's operation. It typically includes directives that specify how the server should behave, what resources it should serve, and how to handle requests. Here’s a breakdown of its components:

![image](https://github.com/user-attachments/assets/bd6d2fc4-366c-4751-8481-68ca7013be5d)

## **Key Components of a Web Server Configuration File**

- **Server Directives**: These are global settings that apply to the server as a whole. Common directives include:
    - **`listen`**: Specifies the port on which the server listens for incoming connections.
    - **`server_name`**: Defines the hostname or domain name that the server responds to.
    - **`root`**: Indicates the root directory from which files will be served.
- **Location Blocks**: These define specific settings for particular paths or resources within the server. Each block can have its own directives, such as:
    - **`location`**: Specifies a particular URL path and can contain nested directives.
    - **`proxy_pass`**: Used to forward requests to another server.
    - **`error_page`**: Defines custom error pages for specific HTTP status codes.
- **Handling Requests**: The configuration file may also specify how to handle different HTTP methods (GET, POST, DELETE) and how to process scripts (e.g., using CGI).

### The Parsing of The Configuration File:

- To illustrate the parsing logic in this project as a schema, here's a breakdown of the steps :
    1. **File Reading**:
        - Input: Configuration file lines.
        - Action: Each line is read from the file, and empty or comment lines are skipped.
    2. **Server Block Parsing**:
        - Condition: When `server` block starts (`{`), a new server is created.
        - The parser enters a new server context and starts collecting directives within this block.
    3. **Location Block Parsing**:
        - Condition: When `location` block starts (`{`), a new location block is initiated for that server.
        - All location-specific directives are collected until the block ends (`}`).
    4. **Directive Handling**:
        - Every line in the block is checked to see if it's a valid directive (e.g., `listen`, `host`, `root`, etc.).
        - Directives are added to the respective server's or location's directive map.
    5. **Validation Logic**:
        - The parsed directives are validated:
            - **Port Check**: Ensures the port is within the valid range (1025-65535).
            - **Duplicate Location Check**: Ensures no duplicate location blocks exist.
            - **Return Directive Check**: Ensures the `return` directive is valid and the status code is between 300-399.
    6. **Socket Setup**:
        - For each server, sockets are created based on the parsed `listen` and `host` directives.
        - A connection is bound to the parsed IP address (after converting domain to IPv4).
    7. **Override Locations**:
        - Server-level directives are inherited by location blocks unless explicitly overridden within the location.
    8. **Error Handling**:
        - If any validation or parsing error occurs, an exception is thrown with a specific message about the error.

- After the parsing implementation, we get to have at the end a vector of servers, and each server is compounded as this diagram illustrates :

 

```markdown
+---------------------------------------------------+
|                     Server                       |
+---------------------------------------------------+
|       Private                                     |
|  +---------------------------------------------+  |
|  | socketD : int                               |  |
|  +---------------------------------------------+  |
|  | serverAddress : sockaddr_in                 |  |
|  +---------------------------------------------+  |
|  | directives : map<string, string>            |  |
|  | +----------------------------+             |  |
|  | | Key        | Value          |             |  |
|  | +----------------------------+             |  |
|  | | directive1 | value1         |             |  |
|  | | directive2 | value2         |             |  |
|  | +----------------------------+             |  |
|  +---------------------------------------------+  |
|  | locationsBlock : vector<map<string, string>> |  |
|  | +----------------------------+-------------+  |
|  | | Location 1:                 |             |  |
|  | | +----------------------+    |             |  |
|  | | | Key        | Value    |    |             |  |
|  | | +----------------------+    |             |  |
|  | | | path       | /home    |    |             |  |
|  | | | method     | GET      |    |             |  |
|  | +----------------------------+-------------+  |
|  | | Location 2:                                 |  |
|  | | +----------------------+                  |  |
|  | | | Key        | Value    |                  |  |
|  | | +----------------------+                  |  |
|  | | | path       | /login   |                  |  |
|  | | | method     | POST     |                  |  |
|  | +----------------------------+-------------+  |
|  +---------------------------------------------+  |
|  | pages : map<string, map<int, string>>       |  |
|  +---------------------------------------------+  |
+---------------------------------------------------+

+---------------------------------------------------+
|                    Public                         |
+---------------------------------------------------+
|  - preHost : string                               |
|  - prePort : string                               |
|  - duplicated : bool                              |
+---------------------------------------------------+

+---------------------------------------------------+
|                   Methods                         |
+---------------------------------------------------+
|  - bindSockets()                                  |
|  - listenToIncomingConxs()                        |
|  - getSocketDescriptor()                          |
|  - getdirectives()                                |
|  - getPages()                                     |
|  - getlocationsBlock()                            |
|  - setDirectives()                                |
|  - setLocationBlock()                             |
+---------------------------------------------------+

+---------------------------------------------------+
|               Static Methods                      |
+---------------------------------------------------+
|  - parsingFile(string s)                          |
|  - overrideLocations(Server &s)                   |
|  - fillErrorPages(Server &s)                      |
|  - runServers(vector<Server> &servers)            |
+---------------------------------------------------+

```

### System Calls Overview:

![image (1)](https://github.com/user-attachments/assets/f8ea1829-fa2d-413a-854d-76e53fa92674)


- It’s important to mention before moving forward these following system calls that are essential for setting up and running the servers. These calls are presented in roughly the order you'll need them in your program:
- All the notes down have been taken from a book that I highly recommend since it illustrates each step with examples : [https://beej.us/guide/bgnet/html/](https://beej.us/guide/bgnet/html//)

### **1 -`getaddrinfo()` – Setting Up the Address**

`getaddrinfo()` helps you set up the necessary structures for socket communication. It is a critical step to resolve hostnames and prepare the address structures for your server to bind to.

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int getaddrinfo(const char *node,     // e.g. "www.example.com" or IP
                const char *service,  // e.g. "http" or port number
                const struct addrinfo *hints,
                struct addrinfo **res);
```

### **2 -`socket()` – Get the File Descriptor**

`socket()` creates a new socket and returns its file descriptor. This file descriptor is your handle for subsequent operations.

- **Note**: By itself, the socket is incomplete. It needs further setup with system calls such as `bind()` or `connect()`.

```c
int socket(int domain, int type, int protocol);
```

### **3 -`bind()` – Assigning a Port**

The `bind()` call associates your socket with a specific local port, enabling the kernel to route incoming packets to the correct process.

```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

**Tip**: You might not need to call `bind()` if you're connecting to a remote machine and don't care about the local port—`connect()` will bind for you if necessary.

### **4 -`listen()` – Waiting for Connections**

`listen()` marks your socket as a passive one, meaning it will wait for incoming connection requests.

```c
int listen(int sockfd, int backlog);
```

- `backlog`: The maximum number of connections that can be queued. Once the limit is reached, additional requests will be refused.

### **5 -`accept()` – Accept Incoming Connections**

`accept()` pulls the next pending connection from the queue (created by `listen()`), returning a new socket descriptor for this connection.

- The original socket remains open and can continue accepting new connections.

```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

### **6 -`send()` and `recv()` – Data Transmission**

Once a connection is established, you can use `send()` and `recv()` to communicate over the socket.

- **`send()`** transmits data.
- **`recv()`** reads incoming data.

```c
int send(int sockfd, const void *msg, int len, int flags);
int recv(int sockfd, void *buf, int len, int flags);
```

**Note**: Both `send()` and `recv()` can return fewer bytes than requested, meaning you may need to manage the rest manually. Additionally, `recv()` returning `0` means the connection has been closed by the remote side.

### **7 -`close()`** and **`shutdown()`**

- this will prevent anymore reads and writes to the socket. Anyone attempting to read or write the socket on the remote end will receive an error
- Just in case you want a little more control over how the socket closes, you can use the `shutdown()` function. It allows you to cut off communication in a certain direction, or both ways (just like `close()` does).

```c
close(sockfd);
int shutdown(int sockfd, int how); 
```

- Once all the servers are ready for listening to the incoming requests, we have explore I/O multiplexing

# II - **Slightly Advanced Techniques (I/O Multiplexing: The `select` and `poll` Functions)**

- Now that we’ve covered the basics of Unix network programming, it’s time to take things up a notch. If you’re here, congratulations—you’ve mastered the foundational elements! In this section, we’ll explore I/O multiplexing, a crucial concept for handling multiple connections simultaneously.
- When writing network applications, you often need to manage several sockets at once—whether it’s handling multiple client connections or dealing with various I/O streams. That’s where I/O multiplexing with the `select()` system call comes into play.
- The `select()` system call allows a program to monitor multiple file descriptors (like sockets) simultaneously to see if they are ready for I/O operations (e.g., reading or writing). Instead of blocking on one socket until it’s ready, `select()` can watch multiple sockets and tell you which ones are ready for interaction, letting you handle multiple connections efficiently in a single-threaded program.

![image (2)](https://github.com/user-attachments/assets/d627380f-adf9-4f39-b984-2917ff30430b)

**What `select()` Does:**

- **Monitors Multiple Sockets:** `select()` can watch several sockets at the same time to see if they're ready for reading, writing, or have any exceptions.
- **Prevents Blocking:** Instead of your program getting stuck waiting on one socket, `select()` allows you to handle multiple sockets without blocking the entire program.
- **Efficient Resource Use:** By knowing exactly which sockets are ready, your server can manage resources more effectively without wasting CPU time.

**Basic Usage of `select()`:**

```c
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
// Function prototype

int select(int numfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout);
```

- **Parameters:**
    - `numfds`: The highest-numbered file descriptor plus one.
    - `readfds`: Set of sockets to check for readability.
    - `writefds`: Set of sockets to check for writability.
    - `exceptfds`: Set of sockets to check for exceptions.
    - `timeout`: Maximum time `select()` should wait before returning.

**How It Works:**

1. **Initialize File Descriptor Sets:**
    - Use macros to add or remove sockets from the sets.
    - Example macros:
        - `FD_SET(fd, &set)`: Add a socket to the set.
        - `FD_CLR(fd, &set)`: Remove a socket from the set.
        - `FD_ISSET(fd, &set)`: Check if a socket is part of the set.
        - `FD_ZERO(&set)`: Clear all sockets from the set.
2. **Call `select()`:**
    - `select()` will block until one of the specified sockets is ready or the timeout is reached.
3. **Handle Ready Sockets:**
    - After `select()` returns, use `FD_ISSET()` to determine which sockets are ready for reading, writing, or have exceptions.
    - Process each ready socket accordingly, and here comes the third big part which is parsing request and generating responses

# III- Parse Request [GET, POST, DELETE] / Generating Response :

### 1. Request Parsing:

![image (3)](https://github.com/user-attachments/assets/7040e3d6-cb80-40d1-9dbe-b02182825945)

Once a request is received from the client, the server must parse it and ensure it adheres to the HTTP RFC standards, specifically **RFC 2616**, which defines the rules and conventions for handling HTTP/1.1 requests. RFCs (Request for Comments) are a series of standards and protocols developed for the internet, providing a set of guidelines to ensure interoperability and consistency across systems.

In this case, the request parsing process was performed following the guidelines of **RFC 2616**, which specifies the format, structure, and behavior of HTTP requests. However, this schema was enough for parsing the request

![webserv-request-flow](https://github.com/user-attachments/assets/b5170df5-5b70-4600-845a-9e5abedd1277)

The parsing process takes care of:

- **CRLF Handling**: Each line of the request must end with CRLF (Carriage Return Line Feed `\r\n`). To signal the end of the request headers, two consecutive CRLFs (`\r\n\r\n`) are required.
- **HTTP Method Validation**: The server verifies that the HTTP method used (e.g., GET, POST, DELETE) is valid and supported. If an unsupported method is detected, the server responds with a `405 Method Not Allowed` status code. Additionally, methods that are allowed for specific URIs are checked against defined `allowedMethods` to ensure compliance.
- **URI Processing**: The server processes the URI to determine the exact location of the resource requested by the client. This involves splitting the URI into its constituent parts and verifying whether the requested directory exists on the server.
- **Location Block Matching**: Each request is matched against predefined location blocks, which contain configurations for handling requests to specific URIs. If a suitable block is found, its directives are applied to the request. If not, a default location block is used.
- **Header Validation**: The server checks for required headers, such as `Content-Type`, and ensures they comply with the expected formats. For example, if the `Content-Type` is `multipart/form-data`, the server extracts the boundary value for processing uploads.
- **Error Handling**: The server handles various error conditions, such as unsupported methods, invalid URIs, and missing or misconfigured headers. Appropriate HTTP status codes are returned to the client to indicate the nature of the error.
- **Upload Processing**: If file uploads are permitted, the server verifies that the upload directory exists and is writable. Any errors encountered during this process are managed, ensuring that the client receives meaningful feedback.
- **End of Request**:  Two consecutive CRLFs indicates the end of request header, if the request doesn’t contain it and the request timeout has been reached, the server generates a [408 Request Timeout](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/408) response and sends it to the client.
- To streamline this process, we leverage a `Request` class that handles the parsing of each request, ensuring it adheres to the server’s configuration.

### 2. Response Generation:

After successfully parsing the request, the server verifies its compliance with the configuration and generates an appropriate response.

- **Response Building**: To simplify the response generation process, we implement the **Builder Design Pattern** through the `responseBuilder` class. This design pattern allows flexible construction of HTTP responses by adding headers, body content, and status codes in a step-by-step manner.
- Generating a response can be thought of as a process similar to building a house.
    
    **1- Building the Foundation**: The first step is to define the **status line**, much like laying the foundation of the house. This provides the base for the response, indicating whether the request was successful or encountered an error.
    
    **2- Building the Walls**: Next, you add the **Content-Type**, which defines the type of content in the response, similar to putting up walls in the house.
    
    **3- Adding the Doors and Windows**: By adding headers such as `Location` or setting the `Content-Length`, you're adding doors and windows to the house, specifying entry points and size information.
    
    **4- Adding the Furniture**: The **response body** is like the furniture inside the house, providing the actual content of the response.
    
    **5- Customizations**: Any additional headers or customizations are like decorating the house, making sure it meets the client's specific needs.
    
    **6- Final Assembly**: The `build()` method is where all the components are brought together, much like assembling the entire house and handing over the keys. It combines the status, headers, and body to produce the final HTTP response that will be sent back to the client.
    
- By using this **Builder Design Pattern**, the process of constructing HTTP responses becomes more manageable and flexible, allowing for the customization of each aspect of the response based on the server's configurations.

![image (4)](https://github.com/user-attachments/assets/cb2b0719-d724-43ad-8003-fca7ea366d02)

```c
responseBuilder response;

response.addStatusLine("200 OK")
       .addContentType("text/html")
       .addResponseBody("<html><body>Hello, world!</body></html>")
       .build();
```

# IV - CGI (php, py) and Cookie handling

CGI (Common Gateway Interface) is a standard for interfacing web servers with executable programs. In our application, we handle CGI requests to dynamically generate responses based on user input. Below, we discuss the processes for handling GET and POST requests using CGI.

![image (5)](https://github.com/user-attachments/assets/3dd9c637-5d96-4609-9e5c-6ec25f74482f)

### 1. Handling GET Requests: `handleCgiGet`

The `handleCgiGet` function is responsible for processing GET requests. It works by creating a child process to execute a CGI script and redirecting the input and output between the server and the CGI script.

**Function Signature:**

```cpp

std::pair<std::string, std::string> handleCgiGet(const std::string& file, 
	const std::string& interpreterPath, Request &request);

```

**Key Steps**

- **Forking Process**: A new process is created using `fork()`. If the process creation fails, an error message is logged.
- **Setting Up Environment Variables**: The server sets the necessary environment variables that the CGI script will use, such as `REQUEST_METHOD`, `REQUEST_URI`, and `SERVER_PROTOCOL`.
- **Redirecting Output**: The standard output and error streams of the child process are redirected to a pipe so that the server can read the CGI script’s output.
- **Executing the CGI Script**: The script is executed using the `executeChildProcess` function, with the necessary environment variables passed to it.
- **Reading Response**: After execution, the response is read from the pipe, and the status of the child process is checked. If the child process exits with an error, a warning is logged, and a 500 Internal Server Error status is returned.

**Example:**

```cpp
std::pair<std::string, std::string> response = handleCgiGet("script.cgi", "/usr/bin/python3", request
```

### 2. Handling POST Requests: `handleCgiPost`

The `handleCgiPost` function processes POST requests in a similar manner, but it includes additional handling for POST data.

**Function Signature:**

```cpp

std::pair<std::string, std::string> handleCgiPost(const std::string& file, 
const std::string& interpreterPath, Request &request);

```

**Key Steps:**

- **Temporary File Creation**: The POST data is saved to a temporary file, which allows the CGI script to read the incoming data.
- **Forking Process**: Similar to GET requests, a new process is created. If successful, the input is redirected to read from the temporary file.
- **Setting Up Environment Variables**: Environment variables are set as in the GET request, and the `SCRIPT_FILENAME` variable points to the CGI script.
- **Executing the CGI Script**: The script is executed with the filled environment variables.
- **Response Handling**: After execution, the server reads the response from the pipe. Error handling follows the same logic as in the GET request.

**Example:**

```cpp
std::pair<std::string, std::string> response = handleCgiPost("submit.cgi", "/usr/bin/python3", request)
```

### 3. Cookie Management

Cookies are essential for maintaining state across HTTP requests. Our server can set cookies in the HTTP response headers to store user-specific data. When handling CGI scripts, you can also manage cookies by parsing the `Cookie` header from the request and setting `Set-Cookie` headers in the response.

**Setting Cookies:**

- Cookies can be set by appending `Set-Cookie: <cookie_data>` to the response headers before sending them back to the client.

**Example:**

```cpp
responseHeaders["Set-Cookie"] = "session_id=abc123; Path=/; HttpOnly";
```

Building and Running:

- git clone <GITHUB.LINK>
- cd webserv
- make
- ./webserv <config_file.conf>

Resources:

1- Parse of configuration File: 

- https://www.digitalocean.com/community/tutorials/understanding-nginx-server-and-location-block-selection-algorithms
- https://www.digitalocean.com/community/tutorials/understanding-the-nginx-configuration-file-structure-and-configuration-contexts
- https://nginx.org/en/docs/http/request_processing.html

2- I/O Multiplexing

- https://bogotobogo.com/cplusplus/sockets_server_client.php
- https://beej.us/guide/bgnet/html//#select
- https://notes.shichao.io/unp/ch6/
- https://jameshfisher.com/2017/04/05/set_socket_nonblocking/

3- Parse Request [GET, POST, DELETE] / Generating response

- RFC 2616 : https://datatracker.ietf.org/doc/html/rfc2616
- https://developer.mozilla.org/en-US/docs/Web/HTTP/Overview
- https://en.wikipedia.org/wiki/List_of_RFCs
- https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages
- https://flaviocopes.com/http-response-headers/
- https://refactoring.guru/design-patterns/builder
- https://dev.to/srishtikprasad/builder-design-pattern-3a7j
- Delete HTTP Method : https://medium.com/@noransaber685/exploring-directory-operations-opendir-readdir-and-closedir-system-calls-a8fb1b6e67bb

4- CGI (php and py) and Cookie handling 

- https://forhjy.medium.com/42-webserv-cgi-programming-66d63c3b22db
- https://www.purplepixie.org/cgi/howto.php
- https://cmd.inp.nsk.su/old/cmd2/manuals/unix/UNIX_Unleashed/ch20.htm
