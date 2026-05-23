#pragma once

#ifndef NTEXT_H
#define NTEXT_H

#include "../utils/Text/NString.h"
#include "NObject.h"
#include <SFML/Graphics/Text.hpp>

class NText : public NObject {
private:
public:
	mutable sf::Text sfText; // store text data

	enum AlignOption {
		LeftTop,
		Center
	};
	int align = LeftTop;

	NText();
	explicit NText(const NString& str = {}, AlignOption = LeftTop, unsigned int characterSize = 20UL);
	explicit NText(const NString& str, const sf::Font& font, AlignOption align = LeftTop, unsigned int characterSize = 20UL);
	// NText(const sf::Font& font, unsigned int characterSize = 20UL) :sfText({}, font, characterSize) {}
	explicit NText(const sf::Text& text, AlignOption align = LeftTop);
	NText(const NText&) = default;

	void draw(const NCanvas& canvas) const override;

	void setText(const NString& str) const {
		sfText.setString(str);
	}
};

class NLineText : public NObject {
public:
	std::vector<NString> strings;
	mutable sf::Text sfText; // store text data (except strings)
	size_t lineLimit = 0xffffffff;

	explicit NLineText(const sf::Font& font, unsigned int characterSize = 20UL) : sfText(font, {}, characterSize) {
	}

	void append(const NString& str) {
		strings.push_back(str);
	}

	void clearText() {
		strings.clear();
	}

	void setSize(size_t size) {
		strings.resize(size);
	}

	void setLineLimit(size_t limit) {
		lineLimit = limit;
	}

	NString& getString(size_t i) {
		if (i >= strings.size()) {
			strings.resize(i + 1);
		}
		return strings[i];
	}

	void draw(const NCanvas& canvas) const override;

	NString& operator[](size_t i) {
		return getString(i);
	}
};

#endif