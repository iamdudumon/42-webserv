#!/usr/bin/env python3
import os
import json
import time

UPLOAD_DIR = os.environ.get('UPLOAD_PATH') or './var/www/uploads'
UPLOAD_DIR = os.path.abspath(os.path.expanduser(UPLOAD_DIR))
os.makedirs(UPLOAD_DIR, exist_ok=True)

def print_header(status):
    if status:
        print(f"Status: {status}\r")
    print("Content-Type: application/json\r")
    print("\r")

method = os.environ.get('REQUEST_METHOD', '')
if method != 'GET':
	print_header("405 Method Not Allowed")
	print(json.dumps({"success": False, "error": "GET 메소드만 허용됩니다"}, ensure_ascii=False))
	sys.exit(0)

try:
    files_list = []
    total_size = 0

    if os.path.exists(UPLOAD_DIR):
        for filename in os.listdir(UPLOAD_DIR):
            filepath = os.path.join(UPLOAD_DIR, filename)
            if os.path.isfile(filepath):
                stat_info = os.stat(filepath)
                files_list.append({
                    "name": filename,
                    "size": stat_info.st_size,
                    "modified": stat_info.st_mtime,
                    "modified_str": time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(stat_info.st_mtime))
                })
                total_size += stat_info.st_size

    response = {
        "success": True,
        "files": files_list,
        "total_count": len(files_list),
        "total_size": total_size
    }

    print_header("200 OK")
    print(json.dumps(response, ensure_ascii=False))

except Exception as e:
    print_header("500 Internal Server Error")
    response = {
        "success": False,
        "error": str(e)
    }
    print(json.dumps(response, ensure_ascii=False))