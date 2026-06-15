#pragma once
#include "../Logging/Logger.h"

#include <fstream>
#include <ios>
#include <iosfwd>
#include <string_view>

namespace flx::file {
	inline std::fstream open(const std::filesystem::path& file, std::ios_base::openmode mode = std::ios::in | std::ios::out) {
		std::fstream file1(file, mode);
		if (!file1.is_open()) {
			logger.error_and_throw("can't open file {}", file);
		}
		return file1;
	}
} // namespace Util