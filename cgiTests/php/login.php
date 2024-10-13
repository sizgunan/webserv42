<?php
session_start();

// Function to authenticate user
function authenticateUser($username, $password) {
    // Check if username and password match the expected values
    return ($username === "admin" && $password === "admin");
}


// Check if form is submitted
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // Check if username and password are provided
    if (isset($_POST["username"]) && isset($_POST["password"])) {
        $username = $_POST["username"];
        $password = $_POST["password"];

        // Authenticate user
        if (authenticateUser($username, $password)) {
            // Authentication successful
            $_SESSION["username"] = $username;

            // Set cookies
            setcookie('username', $username, time() + (86400 * 30), "/"); // 86400 = 1 day
            setcookie('loggedIn', true, time() + (86400 * 30), "/");
            // setcookie('username', $username, time() + 30, "/"); // 30 seconds
            // setcookie('loggedIn', true, time() + 30, "/"); // 30 seconds

            header("Location: /Desktop/WebServ/cgiTests/php/login.php");
            exit();
        } else {
            $error_message = "Invalid username or password. Please try again.";
        }
    }
}

// Check if user is already logged in
if (isset($_COOKIE['username']) && isset($_COOKIE['loggedIn'])) {
    $username = $_COOKIE['username'];
    ?>
    <!DOCTYPE html>
    <html>
    <head>
        <title>Login</title>
        <meta charset="UTF-8">
        <link rel="icon" href="/Desktop/WebServ/pages/response_pages/favicon.ico">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <link rel="stylesheet" href="/Desktop/WebServ/pages/styles.css">
    </head>
    <body>
        <div class="header">
            <ul class="headerLinks">
                <li class="headerItem" ><a href="/">Root</a></li>
                <li class="headerItem" ><a href="/Desktop/WebServ/pages/html/upload.html">Upload</a></li>
                <li class="headerItem" ><a href="/Desktop/WebServ/cgiTests/php/example.php">Get</a></li>
                <li class="headerItem" ><a href="/Desktop/WebServ/pages/html/post.html">Post</a></li>
                <li class="headerItem" ><a href="#">Login</a></li>
            </ul>
        </div>
        <div class="container">
            <div class="head">
                <h1 class="bigTitle">
                    Welcome, <font class="special"><?php echo $username; ?></font>
                </h1>
                <a class="basicLink" href="/Desktop/WebServ/cgiTests/php/logout.php">Logout</a>
            </div>
        </div>
    </body>
    </html>
    <?php
    exit();
} else {
    ?>
    <!DOCTYPE html>
    <html>
    <head>
        <title>Login</title>
        <meta charset="UTF-8">
    <link rel="icon" href="/Desktop/WebServ/pages/response_pages/favicon.ico">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <link rel="stylesheet" href="/Desktop/WebServ/pages/styles.css">
    </head>
    <body>
        <div class="header">
            <ul class="headerLinks">
                <li class="headerItem" ><a href="/">Root</a></li>
                <li class="headerItem" ><a href="/Desktop/WebServ/pages/html/upload.html">Upload</a></li>
                <li class="headerItem" ><a href="/Desktop/WebServ/cgiTests/php/example.php">Get</a></li>
                <li class="headerItem" ><a href="/Desktop/WebServ/pages/html/post.html">Post</a></li>
                <li class="headerItem" ><a href="#">Login</a></li>
            </ul>
        </div>
        <div class="container">
            <div class="head">
                <h1 class="bigTitle">Login Form</h1>
            </div>
            <form class="postForm" action="#" method="POST" enctype="application/x-www-form-urlencoded">
                <?php if(isset($error_message)) { ?>
                    <p style="color: red;"><?php echo $error_message; ?></p>
                <?php } ?>
                <label for="username">Username:</label>
                <input type="text" id="username" name="username" required>
                <label for="password">Password:</label>
                <input type="password" id="password" name="password" required>
                <input type="submit" value="Login">
            </form>
        </div>
    </body>
    </html>
<?php
}
?>
