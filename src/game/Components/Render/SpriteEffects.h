#pragma once
#include "RenderComponent.h"
#include "src/utils/Timer.h"
#include "src/utils/Vec2.h"
#include <SFML/Graphics/Sprite.hpp>

struct EffectState {
	float arg{}; // in degrees;
	nvec2 offset;
	nvec2 scale{1, 1};
	float opacity = 1.f;
	EffectState operator*(float w) const {
		return EffectState{
			.arg = w * arg,
			.offset = w * offset,
			.scale = w * scale,
			.opacity = w * opacity};
	}
	EffectState operator+(const EffectState& e) const {
		return EffectState{
			.arg = arg + e.arg,
			.offset = offset + e.offset,
			.scale = scale + e.scale,
			.opacity = opacity + e.opacity};
	}
};

namespace Easing {
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
}; // namespace Easing

class Transition : public BaseEffect {
private:
	EffectState beginState;
	EffectState endState;
	float (*easing_function)(float t) = {};
	Timer timer;
	EffectState currentState;

public:
	Transition(EffectState beginState, EffectState endState, float duration, float (*easing_function)(float) = Easing::linear) : beginState(beginState), endState(endState), easing_function(easing_function) {
		currentState = beginState;
		timer.set(duration);
		timer.start();
	}

	void update(float dt) override {
		if (isDone())
			return;
		timer.update(dt);
		if (!timer.isRunning()) {
			done();
		}
		const float progress = timer.getProgress();
		assertNotNull(easing_function);
		const float w = easing_function(progress);
		currentState = beginState * (1 - w) + endState * w;
	}

	// assume that the origin is the center
	void apply(sf::Sprite& sprite) const override {
		sprite.scale(currentState.scale);
		sprite.rotate(currentState.arg);
		sprite.move(currentState.offset);
		sf::Color color = sprite.getColor();
		color.a = static_cast<sf::Uint8>(std::floorf(currentState.opacity * color.a));
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
	float (*easing_function)(float t) = Easing::linear;

public:
	BouncyMoveEffect(const nvec2& scale1, const nvec2& scale2, float duration) : scale1(scale1), scale2(scale2), duration(duration) {
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
		float k = easing_function(progress);
		currentScale = scale1 * (1 - k) + scale2 * k;
	}

	// assume that the origin is the center
	void apply(sf::Sprite& sprite) const override {
		float height = sprite.getGlobalBounds().height;
		float yoffset = (height * currentScale.y - height) / 2.f;
		sprite.scale(currentScale);
		sprite.move(0.f, -yoffset);
	}
};
