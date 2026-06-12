#pragma once
#include "../Logging/Logger.h"

#include <fstream>
#include <ios>
#include <iosfwd>
#include <string_view>

namespace Util::File {
	inline std::fstream open(std::string_view filename, std::ios_base::openmode mode = std::ios::in | std::ios::out) {
		std::fstream file(filename.data(), mode);
		if (!file.is_open()) {
			LoggerOld::error_and_throw("can't open file {}", filename);
		}
		return file;
	}
} // namespace Util