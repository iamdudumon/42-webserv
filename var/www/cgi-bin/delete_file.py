#!/usr/bin/env python3
import os
import json
import sys
from urllib.parse import parse_qs

UPLOAD_DIR = "./var/www/uploads"

def print_header(status=None):
    if status:
        print(f"Status: {status}\r")
    print("Content-Type: application/json\r")
    print("\r")

method = os.environ.get('REQUEST_METHOD', '')
if method != 'DELETE':
	print_header("405 Method Not Allowed")
	print(json.dumps({"success": False, "error": "DELETE 메소드만 허용됩니다"}, ensure_ascii=False))
	sys.exit(0)

try:
    # QUERY_STRING에서 filename 추출
    query_string = os.environ.get('QUERY_STRING', '')
    params = parse_qs(query_string)
    filename = params.get('filename', [''])[0]

    # 입력값 검증
    if not filename:
        print_header("400 Bad Request")
        print(json.dumps({"success": False, "error": "파일명이 제공되지 않았습니다"}, ensure_ascii=False))
        sys.exit(0)

    if '..' in filename or '/' in filename:
        print_header("400 Bad Request")
        print(json.dumps({"success": False, "error": "잘못된 파일명입니다"}, ensure_ascii=False))
        sys.exit(0)

    filepath = os.path.join(UPLOAD_DIR, filename)

    if not os.path.exists(filepath) or not os.path.isfile(filepath):
        print_header("404 Not Found")
        print(json.dumps({"success": False, "error": f"{filename} 파일을 찾을 수 없습니다"}, ensure_ascii=False))
        sys.exit(0)

    # 정상 삭제
    os.remove(filepath)
    print_header()  # Status 헤더 없이 Content-Type만 출력 (200 OK)
    print(json.dumps({"success": True, "message": f"{filename} 파일이 삭제되었습니다"}, ensure_ascii=False))

except Exception as e:
    print_header("500 Internal Server Error")
    print(json.dumps({"success": False, "error": str(e)}, ensure_ascii=False))