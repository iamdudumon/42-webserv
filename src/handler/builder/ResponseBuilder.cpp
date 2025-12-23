#include "ResponseBuilder.hpp"

#include "AutoIndexBuilder.hpp"
#include "ErrorBuilder.hpp"
#include "FileBuilder.hpp"
#include "RedirectBuilder.hpp"

using namespace handler::builder;

ResponseBuilder::ResponseBuilder() : _defaultBuilder(NULL) {
	_builders[router::RouteDecision::ServeFile] = new FileBuilder();
	_builders[router::RouteDecision::ServeAutoIndex] = new AutoIndexBuilder();
	_builders[router::RouteDecision::Redirect] = new RedirectBuilder();
	_builders[router::RouteDecision::Error] = new ErrorBuilder();
	_defaultBuilder = _builders[router::RouteDecision::Error];
}

ResponseBuilder::~ResponseBuilder() {
	for (std::map<router::RouteDecision::Action, IBuilder*>::iterator it = _builders.begin();
		 it != _builders.end(); ++it) {
		if (it->second && it->second != _defaultBuilder) delete it->second;
	}
	if (_defaultBuilder) delete _defaultBuilder;
}

const IBuilder* ResponseBuilder::selectBuilder(router::RouteDecision::Action action) const {
	std::map<router::RouteDecision::Action, IBuilder*>::const_iterator it = _builders.find(action);

	if (it != _builders.end() && it->second) return it->second;
	return _defaultBuilder;
}

http::Packet ResponseBuilder::build(const http::Packet& req, const router::RouteDecision& decision,
									const config::Config& config) const {
	const IBuilder* builder = selectBuilder(decision.action);
	return builder->build(decision, req, config);
}

std::string ResponseBuilder::buildCgi(const std::string& cgiResult, bool keepAlive) {
	return _cgiBuilder.build(cgiResult, keepAlive);
}
