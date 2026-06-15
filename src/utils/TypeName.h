#pragma once
#include <string_view>

namespace flx {
	template <class Type>
	[[nodiscard]] consteval std::string_view typeFullName() noexcept {
		std::string_view pretty_function{static_cast<const char*>(__FUNCSIG__)};
		size_t pos = pretty_function.find('<');
		pos = pretty_function.find('<', pos + 1);
		pos = pretty_function.find('<', pos + 1);
		const size_t first = pretty_function.find_first_not_of(' ', pos + 1);
		const size_t last = pretty_function.find_last_of('>');
		return pretty_function.substr(first, last - first);
	}

	template <class Type>
	[[nodiscard]] consteval std::string_view typeName() noexcept {
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

	template <class Type>
	[[nodiscard]] consteval std::string_view typeNameShort() noexcept {
		std::string_view name = typeName<Type>();

		const size_t template_pos = name.find('<');
		const size_t head_end = template_pos == std::string_view::npos ? name.size() : template_pos;

		const std::string_view head = name.substr(0, head_end);
		const size_t scope_pos = head.rfind("::");

		if (scope_pos != std::string_view::npos) {
			name.remove_prefix(scope_pos + 2);
		}

		// ReSharper disable once CppDFALocalValueEscapesFunction
		return name;
	}

	namespace type {
		template <class T, class... Ts>
		inline constexpr bool is_one_of_v = (std::is_same_v<T, Ts> || ...);

		template <class>
		inline constexpr bool always_false_v = false;
	} // namespace type
} // namespace flx