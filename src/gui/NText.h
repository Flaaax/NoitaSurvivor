#pragma once

#ifndef NTEXT_H
#define NTEXT_H

#include"NObject.h"
#include"src/utils/NString.h"
#include<SFML/Graphics/Text.hpp>


class NText :public NObject {
private:
public:
	mutable sf::Text sfText;	//store text data

	enum :int {
		LeftTop,
		Center
	};
	int align = LeftTop;

	NText();
	NText(const NString& str, int align = LeftTop, unsigned int characterSize = 20UL);
	NText(const NString& str, const sf::Font& font, int align = LeftTop, unsigned int characterSize = 20UL) :sfText(str.c_str(), font, characterSize), align(align) {
		sfText.setFillColor({});
	}
	//NText(const sf::Font& font, unsigned int characterSize = 20UL) :sfText({}, font, characterSize) {}
	NText(const sf::Text& text) :sfText(text) {}
	NText(const NText&) = default;

	void draw(Renderer& renderer)const override;

	void setText(const NString& str) {
		sfText.setString(str.c_str());
	}

	void setDefaultFont();
};


class NLineText :public NObject {
public:
	std::vector<NString> strings;
	mutable sf::Text sfText;	//store text data (except strings)
	size_t lineLimit = 0xffffffff;

	NLineText()noexcept {}
	NLineText(const sf::Font& font, unsigned int characterSize = 20UL) :sfText({}, font, characterSize) {}

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

	void draw(Renderer& renderer)const override;

	NString& operator[](size_t i) {
		return getString(i);
	}
};


#endif