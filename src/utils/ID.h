#pragma once
#include "Text/CaseConvert.h"
#include "TypeName.h"

#include <string>

namespace Util {
	template <class T>
	std::string makeContentID() {
		constexpr auto fullName = Util::typeName<T>();
		return Util::pascalToSnake(fullName);
	}
} // namespace Util