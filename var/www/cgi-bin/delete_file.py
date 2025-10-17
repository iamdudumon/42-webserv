#!/usr/bin/env python3
import os
import json
import sys

# CGI 헤더
print("Content-Type: application/json")
print()

# 업로드 디렉토리
UPLOAD_DIR = "var/www/uploads"

try:
    # DELETE 데이터 읽기
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    if content_length > 0:
        post_data = sys.stdin.read(content_length) # query string으로 변경
        data = json.loads(post_data)
        filename = data.get('filename', '')
        
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
    else:
        raise ValueError("요청 데이터가 없습니다")
    
    print(json.dumps(response, ensure_ascii=False))

except Exception as e:
    response = {
        "success": False,
        "error": str(e)
    }
    print(json.dumps(response, ensure_ascii=False))
