#pragma once
#include <format>

namespace flx {
	template<class ...Args>
	std::string vformat(std::string_view f, Args&& ...args) {
		return std::vformat(f, std::make_format_args(args...));
	}
}