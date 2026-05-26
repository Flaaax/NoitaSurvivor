#pragma once
#include "src/utils/Container/Map.h"
#include "src/utils/Integers.h"

class LocString {
private:
	Util::StrMap<std::string> vars;
	std::string rawString;

public:
	explicit LocString(std::string_view rawString, Util::StrMap<std::string> vars = {})
		: vars(vars), rawString(std::string(rawString)) {}

	// Overrides the existing pair
	void add(std::string key, std::string var) {
		vars[std::move(key)] = std::move(var);
	}

	std::string compile() const {
		std::string result;
		result.reserve(rawString.size());

		for (u64 i = 0; i < rawString.size();) {
			if (rawString[i] != '{') {
				result += rawString[i++];
				continue;
			}

			const u64 end = rawString.find('}', i + 1);

			// 没找到右括号，直接保留剩余内容
			if (end == std::string::npos) {
				result += rawString.substr(i);
				break;
			}

			std::string key = rawString.substr(i + 1, end - i - 1);

			if (const auto str = vars.try_find(key)) {
				result += *str;
			} else {
				// 找不到变量时保留原样，比如 {enemy}
				result += rawString.substr(i, end - i + 1);
			}

			i = end + 1;
		}

		return result;
	}
};