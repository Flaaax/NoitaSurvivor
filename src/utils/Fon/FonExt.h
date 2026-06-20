#pragma once
#include "Fon.h"
#include "src/utils/Vec2/Vec2.h"

namespace flx::fon {
	inline std::optional<vec2> getIfVec2(const Fon& j, std::string_view key = {}) {
		const auto& jj = j.contains(key) ? j.at(key) : j;
		if (const auto arr = jj.getIfArray<float, 2>()) {
			return vec2{(*arr)[0], (*arr)[1]};
		}
		return {};
	}
} // namespace flx::fon