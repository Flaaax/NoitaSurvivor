#include "NString.h"
#include<boost/locale.hpp>
#pragma execution_character_set("utf-8")


NString::operator sf::String() const {
	auto converted = boost::locale::conv::to_utf<wchar_t>(m_data, "utf-8");
	return sf::String(converted);
}
