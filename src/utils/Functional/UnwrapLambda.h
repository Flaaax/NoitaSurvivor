#pragma once

#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

namespace flx {
	namespace internal {
		template <bool Own, typename F, typename R, typename... Args>
		struct CCallback {
			using FnPtr = R (*)(Args..., void*);
			using Target = std::conditional_t<Own, F, F*>;

			FnPtr fn;
			Target target;

			void* ctx() {
				if constexpr (Own) {
					return static_cast<void*>(std::addressof(target));
				} else {
					return const_cast<void*>(static_cast<const void*>(target));
				}
			}
		};

		template <bool Own, typename F, typename R, typename... Args>
		struct LambdaBridgeImpl {
			using Callback = CCallback<Own, F, R, Args...>;

			static R invoke(Args... args, void* ctx) {
				auto* f = static_cast<F*>(ctx);
				return std::invoke(*f, std::forward<Args>(args)...);
			}

			template <typename G>
			static Callback make(G&& f) {
				if constexpr (Own) {
					return Callback{&invoke, std::forward<G>(f)};
				} else {
					return Callback{&invoke, std::addressof(f)};
				}
			}
		};

		template <bool Own, typename F, typename MemFn>
		struct LambdaBridge;

		template <bool Own, typename F, typename C, typename R, typename... Args>
		struct LambdaBridge<Own, F, R (C::*)(Args...) const>
			: LambdaBridgeImpl<Own, F, R, Args...> {};

		template <bool Own, typename F, typename C, typename R, typename... Args>
		struct LambdaBridge<Own, F, R (C::*)(Args...)>
			: LambdaBridgeImpl<Own, F, R, Args...> {};
	} // namespace internal

	template <typename F>
	auto unwrapLambda(F&& f) {
		constexpr bool Own = !std::is_lvalue_reference_v<F&&>;
		using Obj = std::conditional_t<Own, std::remove_cvref_t<F>, std::remove_reference_t<F>>;
		using Raw = std::remove_cvref_t<F>;
		using MemFn = decltype(&Raw::operator());
		return internal::LambdaBridge<Own, Obj, MemFn>::make(std::forward<F>(f));
	}
} // namespace Util