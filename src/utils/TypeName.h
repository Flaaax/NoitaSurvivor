#pragma once
#include <string_view>

namespace Util {

	template <typename Type>
	[[nodiscard]] inline consteval std::string_view typeFullName() noexcept {
		std::string_view pretty_function{static_cast<const char*>(__FUNCSIG__)};
		size_t pos = pretty_function.find('<');
		pos = pretty_function.find('<', pos + 1);
		pos = pretty_function.find('<', pos + 1);
		const size_t first = pretty_function.find_first_not_of(' ', pos + 1);
		const size_t last = pretty_function.find_last_of('>');
		return pretty_function.substr(first, last - first);
	}

	template <typename Type>
	[[nodiscard]] inline consteval std::string_view typeName() noexcept {
		std::string_view name = typeFullName<Type>();
		constexpr std::string_view class_prefix = "class ";
		constexpr std::string_view struct_prefix = "struct ";

		if (name.starts_with(class_prefix)) {
			name.remove_prefix(class_prefix.size());
		} else if (name.starts_with(struct_prefix)) {
			name.remove_prefix(struct_prefix.size());
		}

		// ReSharper disable once CppDFALocalValueEscapesFunction
		return name;
	}
} // namespace Util