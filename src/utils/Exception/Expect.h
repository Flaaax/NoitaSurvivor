#pragma once
#include <variant>

namespace flx {
	template <class T, class E>
	class Expect {
	private:
		std::variant<T, E> value;

	public:
		template <class... Args>
		explicit Expect(Args&&... args)
			: value(std::forward<Args>(args)...) {}

		T& ok() {

		}
	};
} // namespace flx