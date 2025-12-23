#ifndef HANDLER_BUILDER_RESPONSEBUILDER_HPP
#define HANDLER_BUILDER_RESPONSEBUILDER_HPP

#include <map>

#include "../../config/model/Config.hpp"
#include "../../http/model/Packet.hpp"
#include "Builder.hpp"
#include "CgiBuilder.hpp"

namespace handler {
	namespace builder {
		class ResponseBuilder {
			private:
				std::map<router::RouteDecision::Action, IBuilder*> _builders;
				IBuilder* _defaultBuilder;
				CgiBuilder _cgiBuilder;

				const IBuilder* selectBuilder(router::RouteDecision::Action) const;

			public:
				ResponseBuilder();
				~ResponseBuilder();

				http::Packet build(const http::Packet&, const router::RouteDecision&,
								   const config::Config&) const;
				std::string buildCgi(const std::string&, bool);
		};
	}  // namespace builder
}  // namespace handler

#endif
