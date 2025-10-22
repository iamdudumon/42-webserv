// ChunkedBodyState.hpp
#ifndef HTTP_PARSER_CHUNKEDBODYSTATE_HPP
#define HTTP_PARSER_CHUNKEDBODYSTATE_HPP

#include <cstddef>

#include "ParseState.hpp"

namespace http {
	class ChunkedBodyState : public ParseState {
		private:
			enum Stage {
				ReadSize,
				ReadData,
				ReadCRLF,
				ReadTrailer
			};

			size_t _currentChunkSize;
			Stage _stage;
			bool _done;

			void readChunkSize(Parser*);
			void readChunkData(Parser*);
			void readChunkDelimiter(Parser*);
			void readTrailer(Parser*);

		public:
			ChunkedBodyState();
			virtual ~ChunkedBodyState() {}

			virtual void parse(Parser*);
			virtual void handleNextState(Parser*);
	};
}  // namespace http

#endif
