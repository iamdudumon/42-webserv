#!/usr/bin/env python3
import os
import sys
import cgi
import cgitb

cgitb.enable()

UPLOAD_DIR = "./var/www/uploads"
os.makedirs(UPLOAD_DIR, exist_ok=True)

def print_header(status=None):
    if status:
        print(f"Status: {status}")
    print("Content-Type: text/html")
    print()

try:
    form = cgi.FieldStorage()
    
    # 정상 처리
    if "file" in form:
        file_item = form["file"]
        if file_item.filename:
            filename = os.path.basename(file_item.filename)
            filepath = os.path.join(UPLOAD_DIR, filename)
            data = file_item.file.read()
            with open(filepath, "wb") as f:
                f.write(data)
            print_header()  # Status 헤더 없이 Content-Type만 출력
            print("<html><body>")
            print(f"<h2>✅ 파일 '{filename}' 업로드 성공!</h2>")
            print(f"<p>경로: {filepath}</p>")
            print(f"<p>크기: {os.path.getsize(filepath)} bytes</p>")
            print("</body></html>")
        else:
            print_header("400 Bad Request")
            print("<html><body><h2>❌ 파일명이 없습니다</h2></body></html>")
    else:
        print_header("400 Bad Request")
        print("<html><body><h2>❌ 'file' 필드가 없습니다</h2></body></html>")
except Exception as e:
    print_header("500 Internal Server Error")
    print("<html><body>")
    print(f"<h2>❌ 에러: {e}</h2>")
    import traceback
    print("<pre>")
    traceback.print_exc()
    print("</pre>")
    print("</body></html>")