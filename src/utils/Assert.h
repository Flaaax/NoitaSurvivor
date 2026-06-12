#pragma once
#include "Logging/Logger.h"
#include "Macro.h"
#include <assert.h>
#include <source_location>

inline std::string getAssertMsg(std::string_view msg, const std::source_location& location) {
	return fmt::format("Assertion failed:\nat file: {}\nat line: {}\nat function: {}\n{}",
					   location.file_name(), location.line(), location.function_name(), msg.data());
}

inline void assertWithMsg(bool expected, std::string_view msg, const std::source_location& location = std::source_location::current()) {
	if (!expected) LoggerOld::error_and_throw(getAssertMsg(msg, location));
}

inline void assertNotNull(const void* data, const std::source_location& location = std::source_location::current()) {
	if (!data) LoggerOld::error_and_throw(getAssertMsg("Null pointer detected!", location).c_str());
}

inline void assertValidIndex(size_t index, size_t bound, const std::source_location& location = std::source_location::current()) {
	if (index >= bound) LoggerOld::error_and_throw(getAssertMsg(fmt::format("Index {} out of bound {}", index, bound), location));
}

inline void assertNotEmpty(std::string_view str, const std::source_location& location = std::source_location::current()) {
	if (str.empty()) LoggerOld::error_and_throw(getAssertMsg("Empty string detected!", location));
}

#ifndef MYASSERT

#ifndef _DEBUG
//The expression is expected to be true
#define MYASSERT(expression, message) ((void)0)
#else
#include<source_location>
#include<iostream>
#include<fmt/format.h>
inline void __MyAssert(bool expression, const char* message, const std::source_location location = std::source_location::current()) {
	if (!expression) {
		auto msg = fmt::format("Assertion failed\nfile: {}\nline: {}\nfunction: {}\nmsg: {}\n",
							   location.file_name(), location.line(), location.function_name(), message);
		std::cerr << msg << std::endl;;
		std::cerr << *reinterpret_cast<char*>(NULL);
		assert(expression);
	}
}
//assert when false (include function detail)
#define MYASSERT(expression, message) __MyAssert(bool(expression), (message))
#endif //ifndef _DEBUG

#endif //ifndef MYASSERT