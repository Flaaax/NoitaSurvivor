#pragma once
#include "../Logging/Logger.h"

#include <fstream>
#include <ios>
#include <iosfwd>
#include <string_view>

namespace flx::file {
	inline std::fstream open(std::string_view filename, std::ios_base::openmode mode = std::ios::in | std::ios::out) {
		std::fstream file(filename.data(), mode);
		if (!file.is_open()) {
			logger.error_and_throw("can't open file {}", filename);
		}
		return file;
	}
} // namespace Util