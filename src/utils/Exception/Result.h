#pragma once
#include "Expected.h"
#include "string"

namespace flx {
	template <class T = void>
	using Result = Expected<T, std::string>;
}