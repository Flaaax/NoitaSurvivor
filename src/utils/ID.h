#pragma once
#include "Text/CaseConvert.h"
#include "TypeName.h"

#include <string>

namespace Util {
	template <class T>
	std::string_view makeContentID() {
		constexpr auto fullName = Util::typeName<T>();
		static std::string ret = Util::pascalToSnake(fullName);
		return ret;
	}

	// template <class T>
	// std::string_view makeContentIDFromPtr(const T*) {
	// 	return makeContentID<T>();
	// }
} // namespace Util