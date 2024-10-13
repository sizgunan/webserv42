#!/usr/bin/env python

import cgi

print("Content-Type: text/html\r\n\r\n")
print("<html><body>")
print("<h1>CGI Test</h1>")

form = cgi.FieldStorage()

if form.list:
    if "data" in form:
        print("<p>Received POST data: {}</p>".format(form["data"].value))
    else:
        print("<p>No POST data received.</p>")
else:
    print("<p>No POST data received.</p>")

print("</body></html>")
