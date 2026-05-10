#pragma once
#include<random>
#include<limits>


class MyRandom {
public:
	MyRandom() : rng(rd()) {}

	template<class T>
	T get(T min, T max) {
		if constexpr (std::is_floating_point_v<T>) {
			std::uniform_real_distribution<T> dist(min, max);
			return dist(rng);
		}
		else if constexpr (std::is_integral_v<T>) {
			std::uniform_int_distribution<T> dist(min, max);
			return dist(rng);
		}
		static_assert("No matching type for T");
	}

	float getFloat(auto min, auto max) {
		return get<float>(static_cast<float>(min), static_cast<float>(max));
	}

	bool getBool(float trueRate = 0.5f) {
		std::bernoulli_distribution dist((double)trueRate);
		return dist(rng);
	}

	// Generate a random float between 0 and 1
	float getFloat() {
		return get<float>(0.0f, 1.0f);
	}

private:
	std::random_device rd;				   // Random device to seed the generator
	std::default_random_engine rng;        // Mersenne Twister 19937 generator
};

namespace Util {
	inline static MyRandom random;

	inline float randomScatter(float scatter) {
		return random.get<float>(-scatter / 2.f, scatter / 2.f);
	}
}