#pragma once
#include <format>
#include <string>

namespace Util {
	template<class ...Args>
	std::string format(std::string_view f, Args&& ...args) {
		return std::format(f, std::make_format_args(args...));
	}
}