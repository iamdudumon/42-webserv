#ifndef HANDLER_BUILDER_CGIBUILDER_HPP
#define HANDLER_BUILDER_CGIBUILDER_HPP

#include <string>

namespace handler {
	namespace builder {
		class CgiBuilder {
			public:
				std::string build(const std::string&, bool) const;
		};
	}  // namespace builder
}  // namespace handler

#endif
