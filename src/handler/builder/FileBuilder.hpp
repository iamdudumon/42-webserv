// FileBuilder.hpp
#ifndef HANDLER_BUILDER_FILE_HPP
#define HANDLER_BUILDER_FILE_HPP

#include "Builder.hpp"

namespace handler {
	namespace builder {
		class FileBuilder : public IBuilder {
			public:
				virtual http::Packet build(const router::RouteDecision&, const http::Packet&,
										   const std::vector<config::Config>&) const;
		};
	}  // namespace builder
}  // namespace handler

#endif
