#pragma once
#include<string_view>

namespace Util {

template <typename Type>
[[nodiscard]] inline constexpr std::string_view TypeName() noexcept {
	std::string_view pretty_function{static_cast<const char*>(__FUNCSIG__)};
	size_t pos = pretty_function.find('<');
	pos = pretty_function.find('<', pos + 1);
	pos = pretty_function.find('<', pos + 1);
	auto first = pretty_function.find_first_not_of(' ', pos + 1);
	auto last = pretty_function.find_last_of('>');
	return pretty_function.substr(first, last - first);
}

}