<?php
    $data = $_GET['data'];
    $name = $_GET['name'];

    echo "<!DOCTYPE html>\n";
    echo "<html>\n";
    echo "<head>\n";
    echo "    <meta charset=\"UTF-8\">\n";
    echo "    <link rel=\"icon\" href=\"/Desktop/WebServ/pages/response_pages/favicon.ico\">\n";
    echo "    <title>CGI Example</title>\n";
    echo "    <link rel=\"stylesheet\" href=\"/Desktop/WebServ/pages/styles.css\">\n";
    echo "</head>\n";
    echo "<body>\n";
    echo "<div class=\"header\">\n";
    echo "    <ul class=\"headerLinks\">\n";
    echo "        <li class=\"headerItem\" ><a href=\"/\">Root</a></li>\n";
    echo "        <li class=\"headerItem\" ><a href=\"/Desktop/WebServ/pages/html/upload.html\">Upload</a></li>\n";
    echo "        <li class=\"headerItem\" ><a href=\"#\">Get</a></li>\n";
    echo "        <li class=\"headerItem\" ><a href=\"/Desktop/WebServ/pages/html/post.html\">Post</a></li>\n";
    echo "        <li class=\"headerItem\" ><a href=\"/Desktop/WebServ/cgiTests/php/login.php\">Login</a></li>\n";
    echo "    </ul>\n";
    echo "</div>\n";
    echo "<div class=\"container\">\n";
    echo "    <div class=\"head\">\n";
    echo "        <h1 class=\"bigTitle\">Hello, <font class=\"special\">CGI</font>!</h1>\n";
    echo "        <p>This is an example of a CGI script.</p>\n";
    echo "    </div>\n";
    echo "    <div>\n";
    echo "        <h2>Query String Test</h2>\n";
    echo "        <div class=\"separator\"></div>\n";
    echo "        <p>Data: $data</p>\n";
    echo "        <p>Name: $name</p>\n";
    echo "    </div>\n";
    echo "</div>";
    echo "</body>\n";
    echo "</html>\n";
?>
