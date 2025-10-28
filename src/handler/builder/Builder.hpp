// Builder.hpp
#ifndef HANDLER_BUILDER_HPP
#define HANDLER_BUILDER_HPP

#include <map>

#include "../../config/model/Config.hpp"
#include "../../http/model/Packet.hpp"
#include "../../router/model/RouteDecision.hpp"

namespace handler {
	namespace builder {
		class IBuilder {
			public:
				virtual ~IBuilder() {}
				virtual http::Packet build(const router::RouteDecision&, const http::Packet&,
										   const std::map<int, config::Config>&) const = 0;
		};
	}  // namespace builder
}  // namespace handler

#endif
