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
        print(f"Status: {status}\r")
    print("Content-Type: application/json\r")
    print("\r")

def is_safe_filename(filename):
    if not filename or filename.startswith('.'):
        return False
    if '..' in filename or '/' in filename or '\\' in filename:
        return False
    return True

method = os.environ.get('REQUEST_METHOD', 'GET').upper()
if method != 'POST':
    print_header("405 Method Not Allowed")
    print('{"success": false, "error": "POST 메소드만 허용됩니다"}')
    sys.exit(0)

try:
    form = cgi.FieldStorage()
    if "file" not in form:
        print_header("400 Bad Request")
        print('{"success": false, "error": "파일이 없습니다"}')
        sys.exit(0)
    
    fileitem = form["file"]
    if not fileitem.filename:
        print_header("400 Bad Request")
        print('{"success": false, "error": "유효한 파일명이 아닙니다"}')
        sys.exit(0)
    
    filename = os.path.basename(fileitem.filename)    
    if not is_safe_filename(filename):
        print_header("400 Bad Request")
        print('{"success": false, "error": "허용되지 않는 파일 형식입니다"}')
        sys.exit(0)
    
    filepath = os.path.join(UPLOAD_DIR, filename)
    if os.path.exists(filepath):
        print_header("409 Conflict")
        print('{"success": false, "error": "이미 존재하는 파일명입니다"}')
        sys.exit(0)
    
    # 파일 저장
    with open(filepath, 'wb') as f:
        f.write(fileitem.file.read())
    
    print_header()
    print(f'{{"success": true, "message": "파일이 업로드되었습니다", "filename": "{filename}"}}')

except Exception as e:
    print_header("500 Internal Server Error")
    print(f'{{"success": false, "error": "서버 오류가 발생했습니다"}}')
