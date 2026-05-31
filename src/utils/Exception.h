#pragma once
// #include "Text/Format.h"
//
// #include <source_location>
// #include <stdexcept>
//
// class NException : public std::exception {
// public:
// 	explicit NException(std::string_view str, const std::source_location location = std::source_location::current())
// 	: exception(("Exception at:" + location_string(location, str)).data()) {
// 	}
//
// 	static std::string location_string(const std::source_location location, std::string_view msg = {}) {
// 		if (!msg.empty()) {
// 			return Util::format("file: {}\nline: {}\nfunction: {}\nmsg: {}", location.file_name(), location.line(), location.function_name(), msg);
// 		}
// 		return Util::format("file: {}\nline: {}\nfunction: {}\n", location.file_name(), location.line(), location.function_name());
// 	}
// };
//
// class NMethodNotImplementedException : public NException {
// public:
// 	explicit NMethodNotImplementedException(const std::source_location location = std::source_location::current())
// 		: NException("Method not implemented", location) {
// 	}
// };