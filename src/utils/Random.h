#pragma once
#include "Integers.h"

#include <random>

namespace flx {
	class Random {
	private:
		std::random_device rd;			// Random device to seed the generator
		std::default_random_engine rng; // Mersenne Twister 19937 generator

	public:
		Random() : rng(rd()) {}

		template <class T>
		T nextVal(T min, T max) {
			if constexpr (std::is_floating_point_v<T>) {
				std::uniform_real_distribution<T> dist(min, max);
				return dist(rng);
			} else if constexpr (std::is_integral_v<T>) {
				std::uniform_int_distribution<T> dist(min, max);
				return dist(rng);
			}
			static_assert("No matching type for T");
			return {};
		}

		float nextFloat(float min = 0.f, float max = 1.f) {
			return nextVal<float>(min, max);
		}

		bool nextBool(float p = 0.5f) {
			std::bernoulli_distribution dist(p);
			return dist(rng);
		}
	};

	inline static Random random;

	inline float randomScatter(float scatter) {
		return random.nextVal<float>(-scatter / 2.f, scatter / 2.f);
	}

	inline float pseudoRandomSigned(u32 seed) {
		seed ^= seed >> 16u;
		seed *= 0x7feb352du;
		seed ^= seed >> 15u;
		seed *= 0x846ca68bu;
		seed ^= seed >> 16u;

		constexpr auto mask = 0x00ffffffu;
		constexpr auto denominator = static_cast<float>(mask);
		const auto normalized = static_cast<float>(seed & mask) / denominator;

		return normalized * 2.f - 1.f;
	}
} // namespace flx