#pragma once
#include "Traits/Traits.h"

#include <type_traits>

namespace flx {
	// For enum/integer flags whose values are already bit masks:
	// Fly = 1 << 0, Swim = 1 << 1, Dash = 1 << 2.
	template <class Tag_t>
		requires std::is_integral_v<Tag_t> || std::is_enum_v<Tag_t>
	class FlagSet {
	private:
		using T = std::conditional_t<std::is_enum_v<Tag_t>, std::underlying_type_t<Tag_t>, Tag_t>;
		T tags{};

	public:
		bool has(Tag_t tag) const {
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

		bool operator==(const FlagSet& other) const {
			return tags == other.tags;
		}

		T flatten() const {
			return tags;
		}
	};

	// For enum values that are contiguous indices: A = 0, B = 1, C = 2...
	// Internally stores them as bits: A -> 1, B -> 2, C -> 4 ...
	template <traits::enum_v Enum, u64 validBits_ = traits::max_digits>
	struct EnumSet {
	public:
		using Underlying = std::underlying_type_t<Enum>;
		using Bits = std::make_unsigned_t<Underlying>;
		static constexpr u64 validBits = std::min(validBits_, traits::digits<Bits>);

		Bits bits{};

		constexpr EnumSet() noexcept = default;

		constexpr explicit(false) EnumSet(Enum e) noexcept
			: bits(this->bit(e)) {}

		constexpr EnumSet(std::initializer_list<Enum> es) noexcept {
			for (auto e : es) {
				this->add(e);
			}
		}

		explicit constexpr EnumSet(Bits bits) : bits(bits) {}

		static constexpr Bits bit(Enum value) noexcept {
			return Bits{1} << static_cast<Bits>(value);
		}

		constexpr bool has(Enum value) const noexcept {
			return (bits & this->bit(value)) != Bits{0};
		}

		constexpr EnumSet& add(Enum value) noexcept {
			bits |= this->bit(value);
			return *this;
		}

		constexpr EnumSet& remove(Enum value) noexcept {
			bits &= ~this->bit(value);
			return *this;
		}

		constexpr EnumSet& set(Enum value, bool enabled = true) noexcept {
			if (enabled) {
				add(value);
			} else {
				remove(value);
			}
			return *this;
		}

		constexpr EnumSet& clear() noexcept {
			bits = Bits{0};
			return *this;
		}

		constexpr static EnumSet all(bool enabled = true) noexcept {
			if (!enabled) {
				return {};
			}

			if constexpr (validBits == 0) {
				return {};
			} else if constexpr (validBits >= std::numeric_limits<Bits>::digits) {
				return EnumSet(~Bits{0});
			} else {
				return EnumSet((Bits{1} << validBits) - Bits{1});
			}
		}

		constexpr Bits flatten() const noexcept {
			return bits;
		}

		constexpr EnumSet& operator|=(EnumSet other) noexcept {
			bits |= other.bits;
			return *this;
		}

		constexpr EnumSet& operator&=(EnumSet other) noexcept {
			bits &= other.bits;
			return *this;
		}

		constexpr EnumSet& operator-=(EnumSet other) noexcept {
			bits &= ~other.bits;
			return *this;
		}
	};
} // namespace flx
