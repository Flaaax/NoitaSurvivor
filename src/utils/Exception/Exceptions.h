#pragma once
#include <stdexcept>

namespace flx::except {
	class Exception : public std::runtime_error {
	public:
		explicit Exception(std::string_view msg)
			: runtime_error(msg.data()) {}
	};

	class LogThrow : public Exception {
	public:
		explicit LogThrow(std::string_view msg)
			: Exception(msg) {}
	};
} // namespace flx::except