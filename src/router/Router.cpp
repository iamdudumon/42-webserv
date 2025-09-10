// Router.cpp
#include "Router.hpp"

#include <algorithm>

#include "utils/fs.hpp"
#include "utils/mime.hpp"
#include "utils/uri.hpp"

const Config* Router::selectServer(const std::vector<Config>& servers, int localPort) const {
	for (size_t i = 0; i < servers.size(); ++i) {
		if (servers[i].getListen() == localPort) return &servers[i];
	}
	return servers.empty() ? NULL : &servers[0];
}

std::string Router::bestLocationPrefix(const Config& server, const std::string& uriPath) const {
	const std::map<std::string, ConfigLocation>& locs = server.getLocation();
	std::string best = "/";
	size_t bestLen = 0;

	for (std::map<std::string, ConfigLocation>::const_iterator it = locs.begin(); it != locs.end();
		 ++it) {
		const std::string& key = it->first;
		if (key.size() <= uriPath.size() && uriPath.compare(0, key.size(), key) == 0) {
			if (key.size() > bestLen) {
				best = key;
				bestLen = key.size();
			}
		}
	}
	if (bestLen == 0 && !server.getLocation().empty()) {
		// 기본 "/"가 있다면 사용
		if (server.getLocation().find("/") != server.getLocation().end()) return std::string("/");
		// 없다면 첫 번째 것을 사용
		return server.getLocation().begin()->first;
	}

	return best;
}

RouteDecision Router::route(const HttpPacket& req, const std::vector<Config>& servers,
							int localPort) const {
	RouteDecision d;
	d.status = HTTP::StatusCode::OK;

	if (!req.isRequest()) {
		d.action = RouteDecision::Error;
		d.status = HTTP::StatusCode::BadRequest;
		return d;
	}

	// 1) 서버 선택
	const Config* server = selectServer(servers, localPort);
	if (!server) {
		d.action = RouteDecision::Error;
		d.status = HTTP::StatusCode::InternalServerError;
		return d;
	}
	d.server = server;

	// 2) URI 경로 정규화 및 location 매칭
	std::string rawPath = RouterUtils::uri::extractPath(req.getStartLine().target);
	std::string normPath = RouterUtils::uri::normalizePath(rawPath);
	std::string locPrefix = bestLocationPrefix(*server, normPath);
	d.location_path = locPrefix;

	// 3) 정책 검사: 메서드/바디 사이즈
	const std::vector<std::string>& allowed = server->getLocationAllowMethods(locPrefix);
	d.allow_methods = allowed;	// 405 시 응답에 사용
	if (!allowed.empty()) {
		bool ok = false;
		const char* m = HTTP::Method::to_string(req.getStartLine().method);
		for (size_t i = 0; i < allowed.size(); ++i) {
			if (allowed[i] == m) {
				ok = true;
				break;
			}
		}
		if (!ok) {
			d.action = RouteDecision::Error;
			d.status = HTTP::StatusCode::MethodNotAllowed;
			return d;
		}
	}

	// 바디 사이즈: POST만 검사 (MVP)
	if (req.getStartLine().method == HTTP::Method::POST) {
		if (req.getBody().getLength() > static_cast<size_t>(server->getClientMaxBodySize())) {
			d.action = RouteDecision::Error;
			d.status = HTTP::StatusCode::RequestEntityTooLarge;
			return d;
		}
	}

	// 4) 경로 해석
	std::string fs_root = server->getLocationRoot(locPrefix);
	std::string rel = normPath.substr(locPrefix.size());
	if (rel.empty()) rel = "/";
	std::string fs_path = RouterUtils::fs::join(fs_root, rel);
	d.fs_root = fs_root;
	d.fs_path = fs_path;

	if (!RouterUtils::fs::safeUnder(fs_root, fs_path)) {
		d.action = RouteDecision::Error;
		d.status = HTTP::StatusCode::Forbidden;
		return d;
	}

	// 5) 리소스 판별
	if (!RouterUtils::fs::exists(fs_path)) {
		d.action = RouteDecision::Error;
		d.status = HTTP::StatusCode::NotFound;
		return d;
	}

	if (RouterUtils::fs::isDir(fs_path)) {
		// 인덱스 파일 우선
		std::string index = server->getLocationIndex(locPrefix);
		if (index.empty()) index = server->getIndex();
		if (!index.empty()) {
			std::string idx_path = RouterUtils::fs::join(fs_path, index);
			if (RouterUtils::fs::exists(idx_path)) {
				d.index_used = index;
				d.fs_path = idx_path;
				d.content_type_hint = RouterUtils::mime::byExtension(index);
				d.action = RouteDecision::ServeFile;
				return d;
			}
		}
		if (server->getAutoIndex()) {
			d.action = RouteDecision::ServeAutoIndex;
			d.status = HTTP::StatusCode::OK;
			return d;
		}
		d.action = RouteDecision::Error;
		d.status = HTTP::StatusCode::Forbidden;
		return d;
	}

	// 파일
	d.content_type_hint = RouterUtils::mime::byExtension(d.fs_path);
	d.action = RouteDecision::ServeFile;
	d.status = HTTP::StatusCode::OK;
	return d;
}
