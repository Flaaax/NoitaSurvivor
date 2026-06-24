#pragma once
#include "Text/CaseConvert.h"
#include "Traits/TypeName.h"

#include <string>

namespace flx {
	template <class T>
	std::string_view makeContentID() {
		constexpr auto fullName = flx::typeNameShort<T>();
		static std::string ret = text::pascalToSnake(fullName);
		return ret;
	}

	// template <class T>
	// std::string_view makeContentIDFromPtr(const T*) {
	// 	return makeContentID<T>();
	// }
} // namespace flx