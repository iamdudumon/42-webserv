#!/usr/bin/env python3
import os
import json
import sys
from urllib.parse import parse_qs

# CGI 헤더
print("Content-Type: application/json")
print()

# 업로드 디렉토리
UPLOAD_DIR = "./var/www/uploads"

try:
    # QUERY_STRING에서 filename 추출
    query_string = os.environ.get('QUERY_STRING', '')
    params = parse_qs(query_string)
    filename = params.get('filename', [''])[0]

    if not filename:
        raise ValueError("파일명이 제공되지 않았습니다")

    # 경로 조작 방지 (보안)
    if '..' in filename or '/' in filename:
        raise ValueError("잘못된 파일명입니다")

    filepath = os.path.join(UPLOAD_DIR, filename)

    if os.path.exists(filepath) and os.path.isfile(filepath):
        os.remove(filepath)
        response = {
            "success": True,
            "message": f"{filename} 파일이 삭제되었습니다"
        }
    else:
        raise FileNotFoundError(f"{filename} 파일을 찾을 수 없습니다")

    print(json.dumps(response, ensure_ascii=False))

except Exception as e:
    response = {
        "success": False,
        "error": str(e)
    }
    print(json.dumps(response, ensure_ascii=False))