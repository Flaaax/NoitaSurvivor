#pragma once
#include "../Traits/TypeArray.h"

#include <type_traits>

namespace flx {
	template <class T>
	struct FuncTraits;

	template <class T>
	struct FuncTraits : FuncTraits<decltype(&std::remove_cvref_t<T>::operator())> {};

	template <class C, class R, class... Args_>
	struct FuncTraits<R (C::*)(Args_...) const> {
		using Return = R;
		using Args = traits::TypeArray<Args_...>;
	};

	template <class C, class R, class... Args_>
	struct FuncTraits<R (C::*)(Args_...)> {
		using Return = R;
		using Args = traits::TypeArray<Args_...>;
	};
} // namespace flx
