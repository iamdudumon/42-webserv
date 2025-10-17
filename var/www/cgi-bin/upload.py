#!/usr/bin/env python3
import os
import sys
import cgi
import cgitb

cgitb.enable()

UPLOAD_DIR = "./var/www/uploads"

# 디렉터리 생성
os.makedirs(UPLOAD_DIR, exist_ok=True)

print("Content-Type: text/html")
print()
print("<html><body>")

try:
    form = cgi.FieldStorage()
    
    print(f"<p>Form keys: {list(form.keys())}</p>")
    
    if "file" in form:
        file_item = form["file"]
        print(f"<p>Has file field</p>")
        print(f"<p>Filename: {file_item.filename}</p>")
        
        if file_item.filename:
            filename = os.path.basename(file_item.filename)
            filepath = os.path.join(UPLOAD_DIR, filename)
            
            data = file_item.file.read()
            print(f"<p>Read {len(data)} bytes</p>")
            
            with open(filepath, "wb") as f:
                f.write(data)
            
            print(f"<h2>✅ 파일 '{filename}' 업로드 성공!</h2>")
            print(f"<p>경로: {filepath}</p>")
            print(f"<p>크기: {os.path.getsize(filepath)} bytes</p>")
        else:
            print("<h2>❌ 파일명이 없습니다</h2>")
    else:
        print("<h2>❌ 'file' 필드가 없습니다</h2>")
        
except Exception as e:
    print(f"<h2>❌ 에러: {e}</h2>")
    import traceback
    print("<pre>")
    traceback.print_exc()
    print("</pre>")

print("</body></html>")