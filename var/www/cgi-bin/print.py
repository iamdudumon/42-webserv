#!/usr/bin/env python3
# filepath: /home/jinsecho/Desktop/fork-42-webserv/var/www/cgi-bin/hello.py
print("Content-Type: text/html")  # ✅ 헤더
print()                           # ✅ 빈 줄
print("<html>")                   # ✅ 바디
print("<body>Hello world</body>")
print("</html>")