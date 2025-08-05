#!/bin/bash

c++ -Wall -Wextra -Werror -std=c++98 \
	test.cpp \
	parser/HttpParser.cpp \
	serializer/HttpSerializer.cpp \
	parser/state/PacketLineState.cpp \
	parser/state/HeaderState.cpp \
	parser/state/BodyState.cpp \
	parser/state/DoneState.cpp \
	model/HttpPacket.cpp \
	model/Header.cpp \
	model/Body.cpp \
	../utils/str_utils.cpp \
	-I../../include -o http_test && ./http_test
