#pragma once
#include "Json.h"
#include "src/utils/Vec2/Vec2.h"

namespace flx::json {
	inline std::optional<vec2> getIfVec2(const Json& j, std::string_view key = {}) {
		const Json& jj = j.contains(key) ? j.at(key) : j;
		if (const auto arr = jj.getIfArray<float, 2>()) {
			return vec2{(*arr)[0], (*arr)[1]};
		}
		return {};
	}
} // namespace flx::json