#pragma once
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

namespace flx {
	template <class>
	class FuncRef;

	// Warning: it does not own the reference
	template <class R, class... Args>
	class FuncRef<R(Args...)> {
	private:
		void* obj_;
		R (*call_)(void*, Args...);

	public:
		FuncRef() = delete;

		template <class F>
			requires(!std::same_as<std::remove_cvref_t<F>, FuncRef> &&
					 std::is_invocable_r_v<R, std::remove_reference_t<F>&, Args...>)
		explicit(false) FuncRef(F&& f) noexcept
			: obj_(const_cast<void*>(static_cast<const void*>(std::addressof(f)))),
			  call_([](void* obj, Args... args) -> R {
				  using Fn = std::remove_reference_t<F>;

				  if constexpr (std::is_void_v<R>) {
					  std::invoke(*static_cast<Fn*>(obj), std::forward<Args>(args)...);
				  } else {
					  return std::invoke(*static_cast<Fn*>(obj), std::forward<Args>(args)...);
				  }
			  }) {}

		R operator()(Args... args) const {
			return call_(obj_, std::forward<Args>(args)...);
		}
	};
} // namespace flx