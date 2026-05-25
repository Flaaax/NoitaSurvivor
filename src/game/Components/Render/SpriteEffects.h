#pragma once
#include "../../../utils/Vec2/Vec2.h"
#include "RenderComponent.h"
#include "src/utils/Assert.h"
#include "src/utils/Timer.h"
#include <SFML/Graphics/Sprite.hpp>

struct EffectState {
	sf::Angle rotation{};
	nvec2 offset;
	nvec2 scale{1, 1};
	float opacity = 1.f;

	EffectState operator*(float w) const {
		return EffectState{
			.rotation = w * rotation,
			.offset = w * offset,
			.scale = w * scale,
			.opacity = w * opacity};
	}

	EffectState operator+(const EffectState& e) const {
		return EffectState{
			.rotation = rotation + e.rotation,
			.offset = offset + e.offset,
			.scale = scale + e.scale,
			.opacity = opacity + e.opacity};
	}
};

namespace Easing {
	using easingFunc = float(float);

	// linear
	inline float linear(float t) {
		return t;
	}

	// fast at end
	inline float quad(float t) {
		return t * t;
	}

	// fast at start
	inline float ease_out_quad(float t) {
		return -t * (t - 2.f);
	}

	// Fast at the start, slower near the end.
	inline float ease_out_cubic(float t) {
		t -= 1.0f;
		return t * t * t + 1.0f;
	}

	inline float ease_in_out_quad(float t) {
		return t < 0.5f ? 2.f * t * t : 1.f - std::pow(-2.f * t + 2.f, 2.f) / 2.f;
	}

	inline float ping_pong(float t) {
		t = std::clamp(t, 0.f, 1.f);
		return 1.f - std::abs(2.f * t - 1.f);
	}
}; // namespace Easing

class Tween : public BaseEffect {
protected:
	EffectState beginState;
	EffectState endState;
	Easing::easingFunc* easingFunc{};
	Timer timer;
	EffectState currentState;
	int repeat{};

public:
	Tween(EffectState beginState, EffectState endState, float duration, int repeat = 0, float (*easing_function)(float) = Easing::linear)
		: beginState(beginState), endState(endState), easingFunc(easing_function), repeat(repeat) {
		currentState = beginState;
		timer.start(duration);
	}

	void update(float dt) override {
		if (isDone())
			return;
		timer.update(dt);
		if (!timer.isRunning()) {
			if (repeat == 0) {
				done();
			}
			if (repeat > 0) {
				repeat--;
			}
		}
		const float progress = timer.getProgress();
		const float w = easingFunc(progress);
		currentState = beginState * (1 - w) + endState * w;
	}

	// assume that the origin is the center
	void apply(sf::Sprite& sprite) const override {
		sprite.scale(currentState.scale);
		sprite.rotate(currentState.rotation);
		sprite.move(currentState.offset);
		sf::Color color = sprite.getColor();
		color.a = static_cast<std::uint8_t>(std::floorf(currentState.opacity * color.a));
		sprite.setColor(color);
	}
};

class BouncyMoveEffect : public BaseEffect {
private:
	float duration;
	float state_time;
	bool direction; // true = forward, false = backward
	nvec2 scale1;	// thin scale
	nvec2 scale2;	// fat scale
	nvec2 currentScale;

public:
	float (*easing_function)(float t) = Easing::linear;

	BouncyMoveEffect(const nvec2& scale1, const nvec2& scale2, float duration) : duration(duration), scale1(scale1), scale2(scale2) {
		state_time = 0.f;
		direction = true;
		currentScale = scale1;
	}

	void update(float dt) override {
		state_time += dt;
		if (state_time >= duration) {
			state_time -= duration;
			direction = !direction;
		}
		float progress = state_time / duration;
		if (!direction)
			progress = 1 - progress;
		const float k = easing_function(progress);
		currentScale = scale1 * (1 - k) + scale2 * k;
	}

	// assume that the origin is the center
	void apply(sf::Sprite& sprite) const override {
		const float height = sprite.getGlobalBounds().size.y;
		const float yoffset = (height * currentScale.y - height) / 2.f;
		sprite.scale(currentScale);
		sprite.move({0.f, -yoffset});
	}
};
