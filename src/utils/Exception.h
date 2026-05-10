#pragma once
#include<stdexcept>
#include"NString.h"
#include <source_location>


class NException :public std::exception {
public:
	NException(const NString& str, const std::source_location location = std::source_location::current()) :
		exception((NString("Exception at:") + location_string(location, str.c_str())).c_str()) {
	}

	static NString location_string(const std::source_location& location, const char* msg = nullptr) {
		if (msg) {
			return NString("file: {}\nline: {}\nfunction: {}\nmsg: {}", location.file_name(), location.line(), location.function_name(), msg);
		}
		return NString("file: {}\nline: {}\nfunction: {}\n", location.file_name(), location.line(), location.function_name());
	}
};

class NMethodNotImplementedException :public NException {
public:
	NMethodNotImplementedException(const std::source_location location = std::source_location::current())
		:NException("Method not implemented", location) {
	}
};