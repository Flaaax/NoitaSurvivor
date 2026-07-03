#include "Fon.h"
#include "../Logging/Logger.h"

#include <cctype>
#include <charconv>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace flx::fon {
	namespace fs = std::filesystem;
	static Logger logger = Logger::makeSync("flx::fon::Fon");
	static constexpr u64 maxDumpIndentWidth = 64;

	namespace {
		void appendIndent(std::string& out, std::string_view indent, u64 depth) {
			for (u64 i = 0; i < depth; ++i) {
				out += indent;
			}
		}

		void appendEscapedString(std::string& out, std::string_view value) {
			out.push_back('"');
			for (const char ch : value) {
				switch (ch) {
				case '"':
					out += "\\\"";
					break;
				case '\\':
					out += "\\\\";
					break;
				case '\b':
					out += "\\b";
					break;
				case '\f':
					out += "\\f";
					break;
				case '\n':
					out += "\\n";
					break;
				case '\r':
					out += "\\r";
					break;
				case '\t':
					out += "\\t";
					break;
				default:
					if (static_cast<unsigned char>(ch) < 0x20) {
						logger.error_and_throw("Cannot dump FON string with unsupported control character");
					}
					out.push_back(ch);
					break;
				}
			}
			out.push_back('"');
		}

		void appendNumber(std::string& out, i64 value) {
			out += std::to_string(value);
		}

		void appendNumber(std::string& out, u64 value) {
			out += std::to_string(value);
		}

		void appendNumber(std::string& out, double value) {
			if (!std::isfinite(value)) {
				logger.error_and_throw("Cannot dump non-finite FON number");
			}

			std::ostringstream ss;
			ss << std::setprecision(17) << value;
			out += ss.str();
		}

		bool isLineBreak(char ch) {
			return ch == '\n' || ch == '\r';
		}

		bool isHorizontalSpace(char ch) {
			return std::isspace(static_cast<unsigned char>(ch)) && !isLineBreak(ch);
		}

		bool isKeyDelimiter(char ch) {
			return ch == '\0' || std::isspace(static_cast<unsigned char>(ch)) || ch == ',' || ch == ':' || ch == '[' || ch == ']' || ch == '{' || ch == '}';
		}

		bool containsCommentStart(std::string_view value) {
			for (u64 i = 0; i + 1 < value.size(); ++i) {
				const char first = value[i];
				const char second = value[i + 1];
				if ((first == '/' && second == '/') || (first == '/' && second == '*')) {
					return true;
				}
			}
			return false;
		}

		bool canDumpBareKey(std::string_view key) {
			if (key.empty() || containsCommentStart(key)) {
				return false;
			}

			for (const char ch : key) {
				if (isKeyDelimiter(ch)) {
					return false;
				}
			}
			return true;
		}

		void appendObjectKey(std::string& out, std::string_view key) {
			if (canDumpBareKey(key)) {
				out += key;
				return;
			}
			appendEscapedString(out, key);
		}

		bool isNumberToken(std::string_view token) {
			if (token.empty()) {
				return false;
			}

			u64 i = 0;
			if (token[i] == '+' || token[i] == '-') {
				++i;
				if (i == token.size()) {
					return false;
				}
			}

			if (i + 2 <= token.size() && token[i] == '0' && (token[i + 1] == 'x' || token[i + 1] == 'X')) {
				i += 2;
				const u64 hexBegin = i;
				while (i < token.size() && std::isxdigit(static_cast<unsigned char>(token[i]))) {
					++i;
				}
				return i != hexBegin && i == token.size();
			}

			bool hasDigits = false;
			while (i < token.size() && std::isdigit(static_cast<unsigned char>(token[i]))) {
				hasDigits = true;
				++i;
			}

			if (i < token.size() && token[i] == '.') {
				++i;
				bool hasFractionDigits = false;
				while (i < token.size() && std::isdigit(static_cast<unsigned char>(token[i]))) {
					hasFractionDigits = true;
					++i;
				}
				if (!hasDigits || !hasFractionDigits) {
					return false;
				}
			} else if (!hasDigits) {
				return false;
			}

			if (i < token.size() && (token[i] == 'e' || token[i] == 'E')) {
				++i;
				if (i < token.size() && (token[i] == '+' || token[i] == '-')) {
					++i;
				}
				const u64 exponentBegin = i;
				while (i < token.size() && std::isdigit(static_cast<unsigned char>(token[i]))) {
					++i;
				}
				if (i == exponentBegin) {
					return false;
				}
			}

			return i == token.size();
		}

	} // namespace

	class Fon::Parser {
	private:
		std::string source;
		fs::path path;
		u64 index = 0;

		// 判断是否已经读到输入末尾。
		bool isEnd() const {
			return index >= source.size();
		}

		// 查看当前字符，末尾返回空字符。
		char peek() const {
			return isEnd() ? '\0' : source[index];
		}

		// 判断当前位置是否匹配指定文本。
		bool startsWith(std::string_view text) const {
			return source.substr(index, text.size()) == text;
		}

		// 判断当前位置是否为 FON 行注释起点。
		bool isLineCommentStart() const {
			return startsWith("//");
		}

		// 判断当前位置是否为 FON 块注释起点。
		bool isBlockCommentStart() const {
			return startsWith("/*");
		}

		// 判断当前位置是否为 FON 注释起点。
		bool isCommentStart() const {
			return isLineCommentStart() || isBlockCommentStart();
		}

		// 消费一个行注释。
		void consumeLineComment() {
			while (!isEnd() && !isLineBreak(peek())) {
				advance();
			}
			consumeLineBreak();
		}

		// 消费一个块注释。
		void consumeBlockComment() {
			advance();
			advance();

			while (!isEnd()) {
				if (startsWith("*/")) {
					advance();
					advance();
					return;
				}
				advance();
			}

			failAndThrow("unterminated block comment");
		}

		// 前进一个字符。
		void advance() {
			if (!isEnd()) {
				++index;
			}
		}

		// 消费一个换行，兼容 CRLF。
		void consumeLineBreak() {
			if (peek() == '\r') {
				advance();
				if (peek() == '\n') {
					advance();
				}
				return;
			}
			if (peek() == '\n') {
				advance();
			}
		}

		// 跳过空白和注释。
		void skipTrivia() {
			while (!isEnd()) {
				if (isLineBreak(peek())) {
					consumeLineBreak();
					continue;
				}

				if (isHorizontalSpace(peek())) {
					advance();
					continue;
				}

				if (isBlockCommentStart()) {
					consumeBlockComment();
					continue;
				}

				if (isLineCommentStart()) {
					consumeLineComment();
					continue;
				}

				break;
			}
		}

		// 读取并校验一个指定字符。
		void expect(char expected) {
			if (peek() != expected) {
				failAndThrow(std::string("expected '") + expected + "'");
			}
			advance();
		}

		// 抛出带行列信息的解析错误。
		[[noreturn]] void failAndThrow(std::string_view message) const {
			u64 line = 1;
			u64 column = 1;
			for (u64 i = 0; i < index && i < source.size(); ++i) {
				if (source[i] == '\n') {
					++line;
					column = 1;
				} else {
					++column;
				}
			}

			logger.error_and_throw("Failed to parse Fon file '{}'\n\tat line {},\n\tcolumn {}:\n\t{}",
								   path.string(),
								   line,
								   column,
								   message);
		}

		// 解析带引号的字符串。
		std::string parseQuotedString() {
			const char quote = peek();
			if (quote != '"' && quote != '\'') {
				failAndThrow("expected string");
			}
			advance();

			std::string result;
			while (!isEnd()) {
				const char ch = peek();
				advance();

				if (ch == quote) {
					return result;
				}

				if (static_cast<unsigned char>(ch) < 0x20) {
					failAndThrow("control character in string");
				}

				if (ch != '\\') {
					result.push_back(ch);
					continue;
				}

				if (isEnd()) {
					failAndThrow("unterminated escape sequence");
				}

				const char escaped = peek();
				advance();
				switch (escaped) {
				case '"':
				case '\'':
				case '\\':
				case '/':
					result.push_back(escaped);
					break;
				case 'b':
					result.push_back('\b');
					break;
				case 'f':
					result.push_back('\f');
					break;
				case 'n':
					result.push_back('\n');
					break;
				case 'r':
					result.push_back('\r');
					break;
				case 't':
					result.push_back('\t');
					break;
				case 'u':
					failAndThrow("unicode escape is not supported; use native UTF-8");
				default:
					failAndThrow("invalid escape sequence");
				}
			}

			failAndThrow("unterminated string");
		}

		// 解析对象键。
		std::string parseObjectKey() {
			if (peek() == '"' || peek() == '\'') {
				return parseQuotedString();
			}

			if (isKeyDelimiter(peek()) || isCommentStart()) {
				failAndThrow("expected object key");
			}

			const u64 begin = index;
			while (!isEnd() && !isKeyDelimiter(peek()) && !isCommentStart()) {
				advance();
			}
			return std::string(source.substr(begin, index - begin));
		}

		// 解析数字 token。
		Fon parseNumber(std::string_view token) const {
			bool isNegative = false;
			bool isFloat = false;
			std::string_view number = token;

			if (!number.empty() && (number.front() == '-' || number.front() == '+')) {
				isNegative = number.front() == '-';
				number.remove_prefix(1);
			}

			if (number.size() >= 2 && number[0] == '0' && (number[1] == 'x' || number[1] == 'X')) {
				number.remove_prefix(2);

				u64 value = 0;
				const auto* const first = number.data();
				const auto* const last = number.data() + number.size();
				const auto [ptr, ec] = std::from_chars(first, last, value, 16);
				if (ec != std::errc() || ptr != last) {
					failAndThrow("invalid hexadecimal number");
				}

				if (isNegative) {
					constexpr u64 minI64Magnitude = static_cast<u64>(std::numeric_limits<i64>::max()) + 1;
					if (value > minI64Magnitude) {
						failAndThrow("invalid hexadecimal number");
					}
					if (value == minI64Magnitude) {
						return Fon(Storage{std::numeric_limits<i64>::min()});
					}
					return Fon(Storage{-static_cast<i64>(value)});
				}

				return Fon(Storage{value});
			}

			for (const char ch : token) {
				if (ch == '.' || ch == 'e' || ch == 'E') {
					isFloat = true;
					break;
				}
			}

			if (isFloat) {
				std::string_view floatNumber = token;
				if (!floatNumber.empty() && floatNumber.front() == '+') {
					floatNumber.remove_prefix(1);
				}

				double value = 0.0;
				const auto* const first = floatNumber.data();
				const auto* const last = floatNumber.data() + floatNumber.size();
				const auto [ptr, ec] = std::from_chars(first, last, value);
				if (ec != std::errc() || ptr != last) {
					failAndThrow("invalid number");
				}
				return Fon(Storage{value});
			}

			if (isNegative) {
				i64 value = 0;
				const auto* const first = token.data();
				const auto* const last = token.data() + token.size();
				const auto [ptr, ec] = std::from_chars(first, last, value);
				if (ec != std::errc() || ptr != last) {
					failAndThrow("invalid number");
				}
				return Fon(Storage{value});
			}

			std::string_view unsignedNumber = token;
			if (!unsignedNumber.empty() && unsignedNumber.front() == '+') {
				unsignedNumber.remove_prefix(1);
			}

			u64 value = 0;
			const auto* const first = unsignedNumber.data();
			const auto* const last = unsignedNumber.data() + unsignedNumber.size();
			const auto [ptr, ec] = std::from_chars(first, last, value);
			if (ec != std::errc() || ptr != last) {
				failAndThrow("invalid number");
			}
			return Fon(Storage{value});
		}

		// 解析无引号值。
		Fon parseBareValue() {
			if (isKeyDelimiter(peek()) || isCommentStart()) {
				failAndThrow("expected value");
			}

			const u64 begin = index;
			while (!isEnd() && !isKeyDelimiter(peek()) && !isCommentStart()) {
				advance();
			}

			const std::string_view token{source.data() + begin, index - begin};
			if (token == "null") {
				return Fon();
			}
			if (token == "true") {
				return Fon(Storage{true});
			}
			if (token == "false") {
				return Fon(Storage{false});
			}
			if (isNumberToken(token)) {
				return parseNumber(token);
			}

			return Fon(std::make_unique<std::string>(token));
		}

		// 解析任意值。
		Fon parseValue() {
			skipTrivia();

			switch (peek()) {
			case '\0':
				failAndThrow("expected value");
			case '"':
			case '\'':
				return Fon(std::make_unique<std::string>(parseQuotedString()));
			case '[':
				return parseArray();
			case '{':
				return parseObject(false);
			case ']':
			case '}':
			case ',':
			case ':':
				failAndThrow("expected value");
			default:
				return parseBareValue();
			}
		}

		// 解析数组。
		Fon parseArray() {
			expect('[');
			skipTrivia();

			auto array = std::make_unique<FonArray>();
			while (true) {
				skipTrivia();
				if (peek() == ']') {
					advance();
					return Fon(std::move(array));
				}
				if (isEnd()) {
					failAndThrow("array ']' missing");
				}
				if (peek() == ',') {
					advance();
					continue;
				}

				array->emplace_back(parseValue());

				skipTrivia();
				if (peek() == ',') {
					advance();
					continue;
				}
				if (peek() == ']') {
					advance();
					return Fon(std::move(array));
				}
				if (isEnd()) {
					failAndThrow("array ']' missing");
				}
			}
		}

		// 解析对象，global 表示顶层隐式对象。
		Fon parseObject(bool global) {
			if (!global) {
				expect('{');
			}

			auto object = std::make_unique<FonObject>();
			while (true) {
				skipTrivia();
				if (isEnd()) {
					if (global) {
						return Fon(std::move(object));
					}
					failAndThrow("object '}' missing");
				}
				if (peek() == '}') {
					if (global) {
						failAndThrow("unexpected '}' on global object");
					}
					advance();
					return Fon(std::move(object));
				}
				if (peek() == ',') {
					advance();
					continue;
				}

				std::string key = parseObjectKey();

				skipTrivia();
				if (peek() == ':') {
					advance();
				}
				object->emplace(std::move(key), parseValue());

				skipTrivia();
				if (peek() == ',') {
					advance();
					continue;
				}
				if (peek() == '}') {
					if (global) {
						failAndThrow("unexpected '}' on global object");
					}
					advance();
					return Fon(std::move(object));
				}
				if (isEnd()) {
					if (global) {
						return Fon(std::move(object));
					}
					failAndThrow("object '}' missing");
				}
			}
		}

	public:
		// 保存待解析内容和错误路径。
		explicit Parser(std::string content, fs::path path = {})
			: source(std::move(content)), path(std::move(path)) {}

		// 解析完整 Fon 文本。
		Fon parse() {
			if (startsWith("\xEF\xBB\xBF")) {
				index += 3;
			}

			skipTrivia();
			Fon result;
			if (isEnd()) {
				result = parseObject(true);
			} else if (peek() == '{') {
				result = parseObject(false);
			} else if (peek() == '[') {
				result = parseArray();
			} else {
				result = parseObject(true);
			}

			skipTrivia();
			if (!isEnd()) {
				failAndThrow("unexpected trailing character");
			}
			return result;
		}
	};

	Fon::Fon(Storage&& storage) noexcept
		: storage(std::move(storage)) {
	}

	Fon::Storage Fon::copyStorage(const Storage& storage) {
		switch (static_cast<Type>(storage.index())) {
		case Type::Null:
			return Storage{std::monostate{}};
		case Type::Bool:
			return Storage{storage.get<bool>()};
		case Type::UInt:
			return Storage{storage.get<u64>()};
		case Type::Int:
			return Storage{storage.get<i64>()};
		case Type::Float:
			return Storage{storage.get<double>()};
		case Type::String:
			return Storage{std::make_unique<std::string>(*storage.get<Unique<std::string>>())};
		case Type::Array:
			return Storage{std::make_unique<FonArray>(*storage.get<Unique<FonArray>>())};
		case Type::Object:
			return Storage{std::make_unique<FonObject>(*storage.get<Unique<FonObject>>())};
		}

		throwInternal("Unknown FON type");
	}

	Fon::Fon(const Fon& other)
		: storage(copyStorage(other.storage)) {
	}

	Fon& Fon::operator=(const Fon& other) {
		if (this == &other) {
			return *this;
		}

		storage = copyStorage(other.storage);
		return *this;
	}

	void* Fon::get(Type type) {
		if (!is(type)) {
			return {};
		}

		switch (type) {
		case Type::Bool:
			return &storage.get<bool>();
		case Type::UInt:
			return &storage.get<u64>();
		case Type::Int:
			return &storage.get<i64>();
		case Type::Float:
			return &storage.get<double>();
		case Type::String:
			return storage.get<Unique<std::string>>().get();
		case Type::Array:
			return storage.get<Unique<FonArray>>().get();
		case Type::Object:
			return storage.get<Unique<FonObject>>().get();
		default:
			return {};
		}
	}

	const void* Fon::get(Type type) const {
		return const_cast<Fon*>(this)->get(type);
	}

	void Fon::throwInternal(std::string_view msg) {
		logger.error_and_throw(msg);
	}

	Result<Fon> Fon::loadFromFile(std::filesystem::path path) {
		const std::ifstream file(path, std::ios::binary);
		if (!file) {
			auto err = vformat("Failed to open Fon file '{}'", path.string());
			// logger.error("{}", err);
			return Unexpected(std::move(err));
		}

		std::ostringstream ss;
		ss << file.rdbuf();

		try {
			return Parser(ss.str(), std::move(path)).parse();
		} catch (const std::exception& e) {
			return Unexpected(e.what());
		}
	}

	Result<Fon> Fon::loadFromString(std::string content) {
		try {
			return Parser(std::move(content)).parse();
		} catch (const std::exception& e) {
			return Unexpected(e.what());
		}
	}

	Fon Fon::array() {
		return Fon(Storage{std::make_unique<FonArray>()});
	}

	Fon Fon::object() {
		return Fon(Storage{std::make_unique<FonObject>()});
	}

	Type Fon::getType() const {
		return static_cast<Type>(storage.index());
	}

	bool Fon::is(Type type) const {
		return storage.index() == static_cast<u64>(type);
	}

	bool Fon::isNull() const {
		return is(Type::Null);
	}

	bool Fon::isBool() const {
		return is(Type::Bool);
	}

	bool Fon::isNumber() const {
		return isInt() || isUInt() || isFloat();
	}

	bool Fon::isInt() const {
		return is(Type::Int);
	}

	bool Fon::isUInt() const {
		return is(Type::UInt);
	}

	bool Fon::isFloat() const {
		return is(Type::Float);
	}

	bool Fon::isArray() const {
		return is(Type::Array);
	}

	bool Fon::isObject() const {
		return is(Type::Object);
	}

	bool Fon::isString() const {
		return is(Type::String);
	}

	const Fon& Fon::operator[](std::string_view key) const {
		return at(key);
	}

	Fon& Fon::operator[](std::string_view key) {
		return at_or_emplace(key);
	}

	const Fon& Fon::at(std::string_view key) const {
		if (!is(Type::Object)) {
			logger.error_and_throw("Not an object");
		}
		return getNoCheck<FonObject>().at(key);
	}

	Fon& Fon::at(std::string_view key) {
		if (!is(Type::Object)) {
			logger.error_and_throw("Not an object");
		}
		return getNoCheck<FonObject>().at(key);
	}

	Fon& Fon::at_or_emplace(std::string_view key, Fon f) {
		if (!is(Type::Object)) {
			logger.error_and_throw("Not an object");
		}
		return getNoCheck<FonObject>().try_emplace(key, std::move(f)).first.second;
	}

	bool Fon::contains(std::string_view key) const {
		if (!is(Type::Object)) {
			return false;
		}
		return getNoCheck<FonObject>().contains(key);
	}

	const Fon::FonObject& Fon::items() const {
		if (!is(Type::Object)) {
			logger.error_and_throw("Not an object");
		}
		return getNoCheck<FonObject>();
	}

	Fon::const_iterator Fon::begin() const {
		if (!is(Type::Array)) {
			logger.error_and_throw("Not an array");
		}
		return getNoCheck<FonArray>().begin();
	}

	Fon::const_iterator Fon::end() const {
		if (!is(Type::Array)) {
			logger.error_and_throw("Not an array");
		}
		return getNoCheck<FonArray>().end();
	}

	const Fon& Fon::operator[](u64 i) const {
		return at(i);
	}

	Fon& Fon::operator[](u64 i) {
		return at(i);
	}

	const Fon& Fon::at(u64 i) const {
		if (!is(Type::Array)) {
			logger.error_and_throw("Not an array");
		}
		return getNoCheck<FonArray>().at(i);
	}

	Fon& Fon::at(u64 i) {
		if (!is(Type::Array)) {
			logger.error_and_throw("Not an array");
		}
		return getNoCheck<FonArray>().at(i);
	}

	bool Fon::contains(u64 i) const {
		if (!is(Type::Array)) {
			return false;
		}
		return getNoCheck<FonArray>().size() > i;
	}

	Fon& Fon::push(Fon f) {
		if (!is(Type::Array)) {
			logger.error_and_throw("Not an array");
		}
		return getNoCheck<FonArray>().emplace_back(std::move(f));
	}

	void Fon::pop() {
		if (!is(Type::Array)) {
			logger.error_and_throw("Not an array");
		}
		getNoCheck<FonArray>().pop_back();
	}

	u64 Fon::size() const {
		if (is(Type::Null)) {
			return 0;
		}
		if (is(Type::Array)) {
			return getNoCheck<FonArray>().size();
		}
		if (is(Type::Object)) {
			return getNoCheck<FonObject>().size();
		}
		return 1;
	}

	void Fon::dumpTo(std::string& out, std::string_view indent, u64 depth, bool global) const {
		const bool pretty = !indent.empty();

		switch (getType()) {
		case Type::Null:
			out += "null";
			break;
		case Type::Bool:
			out += storage.get<bool>() ? "true" : "false";
			break;
		case Type::UInt:
			appendNumber(out, storage.get<u64>());
			break;
		case Type::Int:
			appendNumber(out, storage.get<i64>());
			break;
		case Type::Float:
			appendNumber(out, storage.get<double>());
			break;
		case Type::String:
			appendEscapedString(out, *storage.get<Unique<std::string>>());
			break;
		case Type::Array: {
			const auto& array = *storage.get<Unique<FonArray>>();
			out.push_back('[');
			for (u64 i = 0; i < array.size(); ++i) {
				if (pretty) {
					out.push_back('\n');
					appendIndent(out, indent, depth + 1);
				} else if (i != 0) {
					out.push_back(' ');
				}
				array[i].dumpTo(out, indent, depth + 1, false);
			}
			if (!array.empty() && pretty) {
				out.push_back('\n');
				appendIndent(out, indent, depth);
			}
			out.push_back(']');
			break;
		}
		case Type::Object: {
			const auto& object = *storage.get<Unique<FonObject>>();
			if (!global) {
				out.push_back('{');
			}
			bool first = true;
			for (const auto& [key, value] : object) {
				const u64 keyDepth = global ? depth : depth + 1;
				if (pretty) {
					if (!global || !first) {
						out.push_back('\n');
					}
					appendIndent(out, indent, keyDepth);
				} else if (!first) {
					out.push_back(' ');
				}
				appendObjectKey(out, key);
				out.push_back(' ');
				value.dumpTo(out, indent, keyDepth, false);
				first = false;
			}
			if (!global && object.size() != 0 && pretty) {
				out.push_back('\n');
				appendIndent(out, indent, depth);
			}
			if (!global) {
				out.push_back('}');
			}
			break;
		}
		}
	}

	std::string Fon::dump() const {
		std::string out;
		dumpTo(out, "\t", 0, true);
		return out;
	}

	std::string Fon::dump(u64 indentWidth) const {
		if (indentWidth > maxDumpIndentWidth) {
			logger.error_and_throw("Fon dump indent width is too large: {}", indentWidth);
		}

		std::string out;
		dumpTo(out, std::string(indentWidth, ' '), 0, true);
		return out;
	}

	void Fon::dumpToFile(const std::filesystem::path& file) const {
		std::ofstream out(file, std::ios::binary | std::ios::trunc);
		if (!out) {
			logger.error_and_throw("Failed to open Fon file '{}' for writing", file.string());
		}

		out << dump();
		if (!out) {
			logger.error_and_throw("Failed to write Fon file '{}'", file.string());
		}
	}

	void Fon::dumpToFile(const std::filesystem::path& file, u64 indentWidth) const {
		std::ofstream out(file, std::ios::binary | std::ios::trunc);
		if (!out) {
			logger.error_and_throw("Failed to open Fon file '{}' for writing", file.string());
		}

		out << dump(indentWidth);
		if (!out) {
			logger.error_and_throw("Failed to write Fon file '{}'", file.string());
		}
	}
} // namespace flx::fon
