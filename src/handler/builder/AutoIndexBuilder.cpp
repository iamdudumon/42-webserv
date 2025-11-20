#include "AutoIndexBuilder.hpp"

#include <dirent.h>

#include <sstream>

#include "../../http/response/Factory.hpp"

using namespace handler::builder;

http::Packet AutoIndexBuilder::build(const router::RouteDecision& decision, const http::Packet&,
									 const config::Config&) const {
	std::ostringstream ss;
	ss << "<html><body><h1>Index of " << decision.fsPath << "</h1><ul>";
	DIR* dp = opendir(decision.fsPath.c_str());
	if (dp) {
		const dirent* ent;
		while ((ent = readdir(dp)) != NULL) {
			std::string name = ent->d_name;
			if (name == "." || name == "..") continue;
			ss << "<li>" << name << "</li>";
		}
		closedir(dp);
	}
	ss << "</ul></body></html>";

	http::StatusLine statusLine = {"HTTP/1.1", decision.status,
								   http::StatusCode::to_reasonPhrase(decision.status)};
	http::Packet response(statusLine, http::Header(), http::Body());
	std::string html = ss.str();

	return http::response::Factory::create(http::StatusCode::OK, html, "text/html");
}
