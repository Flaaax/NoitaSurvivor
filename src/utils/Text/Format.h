#pragma once
#include <format>

namespace Util {
	template<class ...Args>
	std::string format(std::string_view f, Args&& ...args) {
		return std::vformat(f, std::make_format_args(args...));
	}
}