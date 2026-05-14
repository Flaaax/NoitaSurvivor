#pragma once

#ifndef NSTRING_H
#define NSTRING_H

// #define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
// #define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#include <SFML/System/String.hpp>
#include <format>
#include <string>

class NString {
private:
	std::string m_data;

public:
	NString() noexcept {
	}
	NString(std::string_view str) : m_data(str) {
	}
	NString(const std::string& str) : m_data(str) {
	}
	NString(const char* str) : m_data(str) {
	}
	template <class... Args>
	explicit NString(const std::format_string<Args...> fmt, Args&&... args) : m_data(std::format(fmt, std::forward<Args>(args)...)) {
	}
	NString(const NString&) = default;
	NString(NString&& other) noexcept : m_data(std::move(other.m_data)) {
	}

	const char* c_str() const {
		return m_data.c_str();
	}

	std::string& string() {
		return m_data;
	}
	const std::string& string() const {
		return m_data;
	}

	NString& operator=(const std::string& str) {
		m_data = str;
		return *this;
	}
	NString& operator=(const char* str) {
		m_data = str;
		return *this;
	}
	NString& operator=(std::string_view str) {
		m_data = str;
		return *this;
	}
	NString& operator=(const NString& other) = default;
	NString& operator=(NString&& other) noexcept {
		m_data = std::move(other.m_data);
		return *this;
	}
	NString& append(const std::string& str) {
		m_data.append(str);
		return *this;
	}
	NString& append(const char* str) {
		m_data.append(str);
		return *this;
	}
	NString& append(std::string_view str) {
		m_data.append(str);
		return *this;
	}
	NString& append(const char c) {
		m_data.push_back(c);
		return *this;
	}

	size_t size() const {
		return m_data.size();
	}

	char& operator[](size_t pos) {
		return m_data[pos];
	}
	const char& operator[](size_t pos) const {
		return m_data[pos];
	}
	char& at(size_t pos) {
		return m_data.at(pos);
	}
	const char& at(size_t pos) const {
		return m_data.at(pos);
	}

	bool operator==(const NString& other) const {
		return m_data == other.m_data;
	}

	NString operator+(const NString& other) const {
		return NString(m_data + other.data());
	}
	NString operator+(const std::string_view other) const {
		NString ret = *this;
		ret.append(other);
		return ret;
	}
	NString operator+(const char other) const {
		NString ret = *this;
		ret.append(other);
		return ret;
	}

	NString& operator+=(const NString& other) {
		m_data.append(other.data());
		return *this;
	}
	NString& operator+=(std::string_view other) {
		m_data.append(other);
		return *this;
	}
	NString& operator+=(const char other) {
		m_data += other;
		return *this;
	}

	operator std::string&() {
		return m_data;
	}
	operator std::string_view() const {
		return m_data;
	}
	operator sf::String() const;

	sf::String toSfString() const {
		return static_cast<sf::String>(*this);
	}

	std::string& data() {
		return m_data;
	}
	const std::string& data() const {
		return m_data;
	}
};

inline NString operator+(std::string_view left, const NString& right) {
	NString ret;
	ret.data() = std::string(left.data()) + right.data();
	return ret;
}

#endif