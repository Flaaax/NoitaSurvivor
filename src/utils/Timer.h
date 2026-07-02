#pragma once
#include "Macro.h"
#include <functional>
#include <type_traits>

namespace flx {
	// Has callback function
	class CTimer {
	private:
		// should be careful about this
		static constexpr float MIN_DURATION = 0.000f;
		static constexpr u64 MAX_FRAME_TRIGGER = 10;

		using callback_t = std::function<void()>;
		callback_t m_onTimeout = {};

		float m_remainingTime = 0;
		float m_duration = MIN_DURATION;
		int m_triggers = 0;
		bool m_isRunning = false;

	public:
		static constexpr int infinite_trigger = -1;

		explicit CTimer() : m_onTimeout({}) {}

		template <typename Func = callback_t>
			requires std::convertible_to<Func, callback_t>
		explicit CTimer(float duration, Func&& onTimeout = {})
			: m_onTimeout(std::forward<Func>(onTimeout)), m_duration(std::max(duration, MIN_DURATION)) {
		}

		template <typename Func = callback_t>
			requires std::convertible_to<Func, callback_t>
		CTimer& set(float duration, Func&& onTimeout = {}) {
			this->m_duration = std::max(duration, MIN_DURATION);
			this->m_onTimeout = std::forward<Func>(onTimeout);
			return *this;
		}

		void setDuration(float duration) {
			m_duration = duration;
		}

		void start(int triggers) {
			reset();
			if (triggers == 0)
				return;
			m_isRunning = true;
			m_triggers = triggers;
			m_remainingTime = m_duration;
		}

		void pause() {
			m_isRunning = false;
		}

		void resume() {
			m_isRunning = true;
		}

		void reset() {
			m_remainingTime = 0;
			m_triggers = 0;
			m_isRunning = false;
		}

		void clear() {
			m_remainingTime = 0;
			m_triggers = 0;
			m_isRunning = false;
			m_onTimeout = {};
			m_duration = MIN_DURATION;
		}

		void update(float deltaTime) {
			if (!m_isRunning)
				return;
			m_remainingTime -= deltaTime;
			u64 currentFrameTrigger = 0;
			while (m_remainingTime <= 0 && currentFrameTrigger < MAX_FRAME_TRIGGER) {
				if (m_onTimeout) {
					m_onTimeout();
				}
				m_triggers--;
				currentFrameTrigger++;
				if (m_triggers == 0) {
					reset();
					break;
				}
				m_remainingTime += m_duration;
			}
		}

		FLX_NODISCARD bool isRunning() const {
			return this->m_isRunning;
		}

		FLX_NODISCARD float getRemainingTime() const {
			return this->m_remainingTime;
		}
	};

	// no callback function
	class Timer {
	private:
		static constexpr float MIN_DURATION = 0.001f;

		float m_remainingTime = 0;
		float m_duration = MIN_DURATION;
		bool m_isRunning = false;

	public:
		explicit Timer() {}

		explicit Timer(float duration) : m_duration(std::max(duration, MIN_DURATION)) {}

		Timer(const Timer&) = default;

		Timer(Timer&& other) noexcept : Timer(other) {
			other.clear();
		}

		Timer& operator=(const Timer&) = default;

		Timer& operator=(Timer&& other) noexcept {
			*this = other;
			other.clear();
			return *this;
		}

		Timer& set(float duration) {
			m_duration = std::max(duration, MIN_DURATION);
			return *this;
		}

		void start() {
			reset();
			m_isRunning = true;
			m_remainingTime = m_duration;
		}

		void start(float duration) {
			set(duration);
			start();
		}

		void pause() {
			m_isRunning = false;
		}

		void resume() {
			m_isRunning = true;
		}

		void reset() {
			m_remainingTime = 0;
			m_isRunning = false;
		}

		void clear() {
			m_remainingTime = 0;
			m_isRunning = false;
			m_duration = MIN_DURATION;
		}

		void update(float deltaTime) {
			if (!m_isRunning)
				return;
			m_remainingTime -= deltaTime;
			while (m_remainingTime <= 0) {
				m_isRunning = false;
				m_remainingTime = 0;
				break;
				// m_remainingTime += m_duration;
			}
		}

		FLX_NODISCARD bool isRunning() const {
			return this->m_isRunning;
		}

		FLX_NODISCARD float remainingTime() const {
			return this->m_remainingTime;
		}

		FLX_NODISCARD float getProgress() const {
			return 1.f - m_remainingTime / m_duration;
		}
	};

} // namespace flx
