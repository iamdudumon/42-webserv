// AutoIndexBuilder.hpp
#ifndef HANDLER_BUILDER_AUTOINDEX_HPP
#define HANDLER_BUILDER_AUTOINDEX_HPP

#include "Builder.hpp"

namespace handler {
	namespace builder {
		class AutoIndexBuilder : public IBuilder {
			public:
				virtual http::Packet build(const router::RouteDecision&, const http::Packet&,
										   const std::vector<config::Config>&) const;
		};
	}  // namespace builder
}  // namespace handler

#endif
