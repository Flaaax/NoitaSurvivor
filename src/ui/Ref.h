#pragma once
#include "src/utils/Pointer.h"

namespace flx::ui {
	class Object;

	template <class T>
	using Ref = SWeak<T>;
	template <class T>
	using CRef = Ref<const T>;
	// using Obj = Ref<Object>;
	// using CObj = CRef<Object>;
} // namespace flx::ui