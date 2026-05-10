#pragma once
#include <type_traits>

namespace Util {
	template<class Tag_t> requires std::is_integral_v<Tag_t> || std::is_enum_v<Tag_t>
	class Tag {
	private:
		using T = std::conditional_t<std::is_enum_v<Tag_t>, std::underlying_type_t<Tag_t>, Tag_t>;
		T tags{};

	public:
		bool has(Tag_t tag) {
			return tags & static_cast<T>(tag);
		}
		void add(Tag_t tag) {
			tags |= static_cast<T>(tag);
		}
		void remove(Tag_t tag) {
			tags &= ~static_cast<T>(tag);
		}
		void clear() noexcept {
			tags = static_cast<T>(0);
		}
	};
}

