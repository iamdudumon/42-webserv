#!/usr/bin/env python3
import os
import json
import time

# CGI 헤더
print("Content-Type: application/json")
print()

# 업로드 디렉토리
UPLOAD_DIR = "./var/www/uploads"

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
    
    print(json.dumps(response, ensure_ascii=False))

except Exception as e:
    response = {
        "success": False,
        "error": str(e)
    }
    print(json.dumps(response))
