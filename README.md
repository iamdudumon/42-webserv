# 42-webserv
42 webserv 과제

```on going...```

### Teams
- [iamdudumon](https://github.com/iamdudumon)
- [dendrog](https://github.com/dendrog)
- [jinseong97](https://github.com/jinseong97)


### Coding Conventions

Wiki: https://github.com/42-dukim/42-webserv/wiki/CODESTYLE.md

### Docker 개발 환경

`docker/` 디렉터리에 `clang-format`과 `cppcheck`를 포함한 개발용 컨테이너 구성이 있습니다. 루트에서 아래 명령으로 빌드 후 필요할 때마다 실행하세요.

```bash
cd docker
docker compose build
docker compose run --rm dev
```

컨테이너 안에서 CI와 동일한 명령(`make`, `clang-format`, `cppcheck` 등)을 실행할 수 있습니다.

### 로컬에서 CI 재현하기 (`act`)

GitHub Actions와 같은 환경에서 워크플로를 빠르게 돌려보고 싶다면 [`act`](https://github.com/nektos/act)를 사용할 수 있습니다.

```bash
# act 설치 (Homebrew 예시)
brew install act

# CI 워크플로 실행 (pull_request 이벤트 시뮬레이션)
act pull_request -W .github/workflows/ci_checks.yml
```

> `act`는 기본적으로 light-weight Docker 이미지를 사용합니다.  
> 필요하다면 `-P ubuntu-latest=ubuntu:22.04`처럼 이미지를 명시해 GitHub Actions와 최대한 동일한 환경으로 맞춰 주세요.
