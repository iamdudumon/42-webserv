// RedirectBuilder.hpp
#ifndef HANDLER_BUILDER_REDIRECT_HPP
#define HANDLER_BUILDER_REDIRECT_HPP

#include "Builder.hpp"

namespace handler {
	namespace builder {
		class RedirectBuilder : public IBuilder {
			public:
				virtual http::Packet build(const router::RouteDecision&, const http::Packet&,
										   const config::Config&) const;
		};
	}  // namespace builder
}  // namespace handler

#endif
