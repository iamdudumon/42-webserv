// Router.cpp
#include "Router.hpp"

#include <map>

#include "utils/fs.hpp"
#include "utils/mime.hpp"
#include "utils/uri.hpp"

using namespace router;

namespace {
	using config::Config;
	using config::LocationConfig;
}

std::string Router::parseQueryString(const std::string& uri) const {
	size_t qpos = uri.find('?');
	if (qpos != std::string::npos) {
		return uri.substr(qpos + 1);
	}
	return "";
}

bool Router::isCgiRequest(const std::string& locationPath, const std::string& fsPath) const {
	if (locationPath != "/cgi-bin" && locationPath != "/cgi-bin/") return false;

	unsigned long dot = fsPath.rfind('.');
	if (dot == std::string::npos) return false;

	std::string ext = fsPath.substr(dot);
	return ext == ".py";
}

bool Router::ensureRequestIsValid(const http::Packet& request, RouteDecision& decision) const {
	if (!request.isRequest()) {
		decision.action = RouteDecision::Error;
		decision.status = http::StatusCode::BadRequest;
		return false;
	}

	decision.status = http::StatusCode::OK;
	return true;
}

void Router::resolveLocation(const Config& server, const http::Packet& request,
							 std::string& normPath, std::string& locPrefix,
							 RouteDecision& decision) const {
	normPath = utils::normalizePath(utils::extractPath(request.getStartLine().target));
	locPrefix = bestLocationPrefix(server, normPath);
	decision.locationPath = locPrefix;
}

bool Router::validateMethod(const Config& server, const http::Packet& request,
							const std::string& locPrefix, RouteDecision& decision) const {
	const std::vector<std::string>& allowed = server.getLocationAllowMethods(locPrefix);
	decision.allowMethods = allowed;
	if (allowed.empty()) return true;

	const char* method = http::Method::to_string(request.getStartLine().method);
	for (size_t i = 0; i < allowed.size(); ++i) {
		if (allowed[i] == method) return true;
	}

	decision.action = RouteDecision::Error;
	decision.status = http::StatusCode::MethodNotAllowed;
	return false;
}

bool Router::validateBodySize(const Config& server, const http::Packet& request,
							  RouteDecision& decision) const {
	if (request.getStartLine().method != http::Method::POST) return true;
	if (request.getBody().getLength() <= static_cast<size_t>(server.getClientMaxBodySize()))
		return true;

	decision.action = RouteDecision::Error;
	decision.status = http::StatusCode::RequestEntityTooLarge;
	return false;
}

bool Router::decideResource(const Config& server, const std::string& normPath,
							const std::string& locPrefix, RouteDecision& decision) const {
	std::string fsRoot = server.getLocationRoot(locPrefix);
	std::string rel = normPath.substr(locPrefix.size());
	if (rel.empty()) rel = "/";
	std::string fsPath = utils::join(fsRoot, rel);

	decision.fsRoot = fsRoot;
	decision.fsPath = fsPath;

	if (!utils::safeUnder(fsRoot, fsPath)) {
		decision.action = RouteDecision::Error;
		decision.status = http::StatusCode::Forbidden;
		return false;
	}

	if (!utils::exists(fsPath)) {
		decision.action = RouteDecision::Error;
		decision.status = http::StatusCode::NotFound;
		return false;
	}

	if (utils::isDir(fsPath)) {
		std::string index = server.getLocationIndex(locPrefix);
		if (index.empty()) index = server.getIndex();
		if (!index.empty()) {
			std::string idxPath = utils::join(fsPath, index);
			if (utils::exists(idxPath)) {
				decision.indexUsed = index;
				decision.fsPath = idxPath;
				decision.contentTypeHint = utils::byExtension(index);
				decision.action = RouteDecision::ServeFile;
				decision.status = http::StatusCode::OK;
				return true;
			}
		}
		if (server.getAutoIndex()) {
			decision.action = RouteDecision::ServeAutoIndex;
			decision.status = http::StatusCode::OK;
			return true;
		}
		decision.action = RouteDecision::Error;
		decision.status = http::StatusCode::Forbidden;
		return false;
	}

	if (isCgiRequest(locPrefix, fsPath)) {
		decision.action = RouteDecision::Cgi;
		return true;
	}

	decision.contentTypeHint = utils::byExtension(fsPath);
	decision.action = RouteDecision::ServeFile;
	decision.status = http::StatusCode::OK;
	return true;
}

const Config* Router::selectServer(const std::map<int, Config>& servers, int localPort) const {
	auto it = servers.find(localPort);
	if (it != servers.end()) return &it->second;
	return servers.empty() ? NULL : &servers.begin()->second;
}

std::string Router::bestLocationPrefix(const Config& server, const std::string& uriPath) const {
	const std::map<std::string, LocationConfig>& locationConfigSet = server.getLocation();
	std::string best = "/";
	size_t bestLen = 0;

	for (std::map<std::string, LocationConfig>::const_iterator it = locationConfigSet.begin();
		 it != locationConfigSet.end(); ++it) {
		const std::string& key = it->first;

		if (key.size() <= uriPath.size() && uriPath.compare(0, key.size(), key) == 0) {
			if (key.size() > bestLen) {
				best = key;
				bestLen = key.size();
			}
		}
	}
	if (bestLen == 0 && !server.getLocation().empty()) {
		if (server.getLocation().find("/") != server.getLocation().end()) return std::string("/");
		return server.getLocation().begin()->first;
	}

	return best;
}

RouteDecision Router::route(const http::Packet& request, const std::map<int, Config>& servers,
							int localPort) const {
	RouteDecision decision;
	if (!ensureRequestIsValid(request, decision)) return decision;
	const Config* server = selectServer(servers, localPort);
	if (!server) {
		decision.action = RouteDecision::Error;
		decision.status = http::StatusCode::InternalServerError;
		return decision;
	}

	decision.server = server;

	std::string normPath;
	std::string locPrefix;

	resolveLocation(*server, request, normPath, locPrefix, decision);

	decision.queryString = parseQueryString(request.getStartLine().target);

	if (!validateMethod(*server, request, locPrefix, decision)) return decision;
	if (!validateBodySize(*server, request, decision)) return decision;
	if (!decideResource(*server, normPath, locPrefix, decision)) return decision;

	return decision;
}
