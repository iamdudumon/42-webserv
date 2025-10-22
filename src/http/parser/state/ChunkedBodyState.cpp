// ChunkedBodyState.cpp
#include "ChunkedBodyState.hpp"

#include <sstream>

#include "../../../utils/str_utils.hpp"
#include "../Parser.hpp"
#include "../exception/NeedMoreInput.hpp"
#include "../exception/ParserException.hpp"
#include "DoneState.hpp"

namespace http {
	ChunkedBodyState::ChunkedBodyState() : _currentChunkSize(0), _stage(ReadSize), _done(false) {}

	void ChunkedBodyState::parse(Parser* parser) {
		if (_done) return;

		while (!_done) {
			switch (_stage) {
				case ReadSize:
					readChunkSize(parser);
					break;
				case ReadData:
					readChunkData(parser);
					break;
				case ReadCRLF:
					readChunkDelimiter(parser);
					break;
				case ReadTrailer:
					readTrailer(parser);
					break;
			}
		}
	}

	void ChunkedBodyState::handleNextState(Parser* parser) {
		if (_done) {
			parser->_packet->applyBodyLength(parser->_packet->getBody().getData().size());
			parser->changeState(new DoneState());
		}
	}

	void ChunkedBodyState::readChunkSize(Parser* parser) {
		std::string line;
		try {
			line = parser->readLine();
		} catch (const NeedMoreInput&) {
			if (parser->inputEnded())
				throw ParserException("Malformed request: chunk size truncated",
									  http::StatusCode::BadRequest);
			throw;
		}
		size_t semicolon = line.find(';');
		if (semicolon != std::string::npos) line.resize(semicolon);

		line.erase(0, line.find_first_not_of(" \t"));
		if (!line.empty()) line.erase(line.find_last_not_of(" \t") + 1);

		if (line.empty())
			throw ParserException("Malformed chunk size line", http::StatusCode::BadRequest);

		std::istringstream iss(line);
		size_t chunkSize = 0;
		iss >> std::hex >> chunkSize;
		if (!iss || !iss.eof())
			throw ParserException("Invalid chunk size", http::StatusCode::BadRequest);

		_currentChunkSize = chunkSize;
		if (_currentChunkSize == 0)
			_stage = ReadTrailer;
		else
			_stage = ReadData;
	}

	void ChunkedBodyState::readChunkData(Parser* parser) {
		std::string data;
		try {
			data = parser->readBytes(_currentChunkSize);
		} catch (const NeedMoreInput&) {
			if (parser->inputEnded())
				throw ParserException("Malformed request: Body incomplete",
									  http::StatusCode::BadRequest);
			throw;
		}

		parser->_packet->appendBody(data.data(), data.size());
		_stage = ReadCRLF;
	}

	void ChunkedBodyState::readChunkDelimiter(Parser* parser) {
		std::string delim;
		try {
			delim = parser->readBytes(2);
		} catch (const NeedMoreInput&) {
			if (parser->inputEnded())
				throw ParserException("Malformed request: chunk delimiter missing",
									  http::StatusCode::BadRequest);
			throw;
		}
		if (delim != "\r\n")
			throw ParserException("Chunk delimiter missing", http::StatusCode::BadRequest);

		_stage = ReadSize;
	}

	void ChunkedBodyState::readTrailer(Parser* parser) {
		std::string line;
		try {
			line = parser->readLine();
		} catch (const NeedMoreInput&) {
			if (parser->inputEnded())
				throw ParserException("Malformed request: chunk trailer truncated",
									  http::StatusCode::BadRequest);
			throw;
		}
		if (line.empty()) {
			_done = true;
		}
	}
}  // namespace http
