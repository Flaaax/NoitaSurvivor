#include "Json.h"

#include "../Integers.h"
#include "../Logging/Logger.h"

#include <cctype>
#include <charconv>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace flx::json {
	namespace fs = std::filesystem;
	static Logger logger = Logger::makeSync("flx::json");
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
						logger.error_and_throw("Cannot dump json string with unsupported control character");
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
				logger.error_and_throw("Cannot dump non-finite json number");
			}

			std::ostringstream ss;
			ss << std::setprecision(17) << value;
			out += ss.str();
		}
	} // namespace

	class Json::Parser {
	private:
		std::string source;
		fs::path path;
		u64 index = 0;

		bool isEnd() const {
			return index >= source.size();
		}

		char peek() const {
			return isEnd() ? '\0' : source[index];
		}

		bool startsWith(std::string_view text) const {
			return source.substr(index, text.size()) == text;
		}

		void advance() {
			if (!isEnd()) {
				++index;
			}
		}

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

			logger.error_and_throw("Failed to parse json file '{}'\n\tat line {},\n\tcolumn {}:\n\t{}",
								   path.string(),
								   line,
								   column,
								   message);
		}

		void skipWhitespaceAndComments() {
			while (!isEnd()) {
				if (std::isspace(static_cast<unsigned char>(peek()))) {
					advance();
					continue;
				}

				if (startsWith("//")) {
					index += 2;
					while (!isEnd() && peek() != '\n') {
						advance();
					}
					continue;
				}

				if (startsWith("/*")) {
					index += 2;
					while (!isEnd() && !startsWith("*/")) {
						advance();
					}
					if (isEnd()) {
						failAndThrow("unterminated block comment");
					}
					index += 2;
					continue;
				}

				break;
			}
		}

		void expect(char expected) {
			if (peek() != expected) {
				failAndThrow(std::string("expected '") + expected + "'");
			}
			advance();
		}

		void parseLiteral(std::string_view literal) {
			if (!startsWith(literal)) {
				failAndThrow(std::string("expected '") + std::string(literal) + "'");
			}
			index += literal.size();
		}

		std::string parseString() {
			expect('"');

			std::string result;
			while (!isEnd()) {
				const char ch = peek();
				advance();

				if (ch == '"') {
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

		Json parseNumber() {
			const u64 begin = index;
			bool isNegative = false;
			bool isFloat = false;

			if (peek() == '-') {
				isNegative = true;
				advance();
			}

			if (peek() == '0') {
				advance();
			} else if (std::isdigit(static_cast<unsigned char>(peek()))) {
				while (std::isdigit(static_cast<unsigned char>(peek()))) {
					advance();
				}
			} else {
				failAndThrow("invalid number");
			}

			if (peek() == '.') {
				isFloat = true;
				advance();
				if (!std::isdigit(static_cast<unsigned char>(peek()))) {
					failAndThrow("expected digit after decimal point");
				}
				while (std::isdigit(static_cast<unsigned char>(peek()))) {
					advance();
				}
			}

			if (peek() == 'e' || peek() == 'E') {
				isFloat = true;
				advance();
				if (peek() == '+' || peek() == '-') {
					advance();
				}
				if (!std::isdigit(static_cast<unsigned char>(peek()))) {
					failAndThrow("expected digit in exponent");
				}
				while (std::isdigit(static_cast<unsigned char>(peek()))) {
					advance();
				}
			}

			const std::string_view number = source.substr(begin, index - begin);
			const auto* const first = number.data();
			const auto* const last = number.data() + number.size();

			if (isFloat) {
				double value = 0.0;
				const auto [ptr, ec] = std::from_chars(first, last, value);
				if (ec != std::errc() || ptr != last) {
					failAndThrow("invalid number");
				}
				return Json(Storage{value});
			}

			if (isNegative) {
				i64 value = 0;
				const auto [ptr, ec] = std::from_chars(first, last, value);
				if (ec != std::errc() || ptr != last) {
					failAndThrow("invalid number");
				}
				return Json(Storage{value});
			}

			u64 value = 0;
			const auto [ptr, ec] = std::from_chars(first, last, value);
			if (ec != std::errc() || ptr != last) {
				failAndThrow("invalid number");
			}
			return Json(Storage{value});
		}

		Json parseArray() {
			expect('[');
			skipWhitespaceAndComments();

			auto array = std::make_unique<JsonArray>();
			if (peek() == ']') {
				advance();
				return Json(std::move(array));
			}

			while (true) {
				array->emplace_back(parseValue());
				skipWhitespaceAndComments();

				if (peek() == ']') {
					advance();
					return Json(std::move(array));
				}

				expect(',');
				skipWhitespaceAndComments();
			}
		}

		Json parseObject() {
			expect('{');
			skipWhitespaceAndComments();

			auto object = std::make_unique<JsonObject>();
			if (peek() == '}') {
				advance();
				return Json(std::move(object));
			}

			while (true) {
				if (peek() != '"') {
					failAndThrow("expected object key");
				}
				std::string key = parseString();

				skipWhitespaceAndComments();
				expect(':');
				skipWhitespaceAndComments();

				object->insert_or_assign(std::move(key), parseValue());
				skipWhitespaceAndComments();

				if (peek() == '}') {
					advance();
					return Json(std::move(object));
				}

				expect(',');
				skipWhitespaceAndComments();
			}
		}

		[[nodiscard]] Json parseValue() {
			skipWhitespaceAndComments();

			switch (peek()) {
			case 'n':
				parseLiteral("null");
				return Json();
			case 't':
				parseLiteral("true");
				return Json(true);
			case 'f':
				parseLiteral("false");
				return Json(false);
			case '"':
				return Json(std::make_unique<std::string>(parseString()));
			case '[':
				return parseArray();
			case '{':
				return parseObject();
			case '\0':
				failAndThrow("expected value");
			default:
				if (peek() == '-' || std::isdigit(static_cast<unsigned char>(peek()))) {
					return parseNumber();
				}
				failAndThrow("expected value");
			}
		}

	public:
		explicit Parser(std::string source, std::filesystem::path path = {})
			: source(std::move(source)),
			  path(std::move(path)) {
		}

		[[nodiscard]] Json parse() {
			// Check utf8 BOM
			if (startsWith("\xEF\xBB\xBF")) {
				index += 3;
			}

			Json result = parseValue();
			skipWhitespaceAndComments();
			if (!isEnd()) {
				failAndThrow("unexpected trailing character");
			}
			return result;
		}
	};

	Json::Json(Storage&& storage) noexcept
		: storage(std::move(storage)) {
	}

	void* Json::get(Type type) {
		if (!is(type)) {
			return {};
		}

		switch (type) {
		case Type::Bool:
			return &std::get<bool>(storage);
		case Type::Int:
			return &std::get<i64>(storage);
		case Type::UInt:
			return &std::get<u64>(storage);
		case Type::Float:
			return &std::get<double>(storage);
		case Type::String:
			return std::get<Unique<std::string>>(storage).get();
		case Type::Array:
			return std::get<Unique<JsonArray>>(storage).get();
		case Type::Object:
			return std::get<Unique<JsonObject>>(storage).get();
		default:
			return {};
		}
	}

	const void* Json::get(Type type) const {
		return const_cast<Json*>(this)->get(type);
	}

	template <class T>
	const T& Json::getNoCheck() const {
		return *static_cast<const T*>(get(getType()));
	}

	void Json::throwInternal(std::string_view msg) {
		logger.error_and_throw(msg);
	}

	Json Json::loadFromFile(std::filesystem::path path) {
		const std::ifstream file(path, std::ios::binary);
		if (!file) {
			logger.error_and_throw("Failed to open json file '{}'", path.string());
		}

		std::ostringstream ss;
		ss << file.rdbuf();

		return Parser(ss.str(), std::move(path)).parse();
	}

	Json Json::loadFromString(std::string content) {
		return Parser(std::move(content)).parse();
	}

	Type Json::getType() const {
		return static_cast<Type>(storage.index());
	}

	bool Json::is(Type type) const {
		return getType() == type;
	}

	bool Json::isNull() const {
		return is(Type::Null);
	}

	bool Json::isBool() const {
		return is(Type::Bool);
	}

	bool Json::isNumber() const {
		return isInt() || isUInt() || isFloat();
	}

	bool Json::isInt() const {
		return is(Type::Int);
	}

	bool Json::isUInt() const {
		return is(Type::UInt);
	}

	bool Json::isFloat() const {
		return is(Type::Float);
	}

	bool Json::isArray() const {
		return is(Type::Array);
	}

	bool Json::isObject() const {
		return is(Type::Object);
	}

	bool Json::isString() const {
		return is(Type::String);
	}

	const Json& Json::operator[](std::string_view key) const {
		return at(key);
	}

	const Json& Json::at(std::string_view key) const {
		if (!is(Type::Object)) {
			logger.error_and_throw("Not an object");
		}
		return getNoCheck<JsonObject>().at(key);
	}

	bool Json::contains(std::string_view key) const {
		if (!is(Type::Object)) {
			return false;
		}
		return getNoCheck<JsonObject>().contains(key);
	}

	const Json::JsonObject& Json::items() const {
		if (!is(Type::Object)) {
			logger.error_and_throw("Not an object");
		}
		return getNoCheck<JsonObject>();
	}

	Json::const_iterator Json::begin() const {
		if (!is(Type::Array)) {
			logger.error_and_throw("Not an array");
		}
		return getNoCheck<JsonArray>().begin();
	}

	Json::const_iterator Json::end() const {
		if (!is(Type::Array)) {
			logger.error_and_throw("Not an array");
		}
		return getNoCheck<JsonArray>().end();
	}

	const Json& Json::operator[](u64 i) const {
		return at(i);
	}

	const Json& Json::at(u64 i) const {
		if (!is(Type::Array)) {
			logger.error_and_throw("Not an array");
		}
		return getNoCheck<JsonArray>().at(i);
	}

	bool Json::contains(u64 i) const {
		if (!is(Type::Array)) {
			return false;
		}
		return getNoCheck<JsonArray>().size() > i;
	}

	u64 Json::size() const {
		if (is(Type::Null)) {
			return 0;
		}
		if (is(Type::Array)) {
			return getNoCheck<JsonArray>().size();
		}
		if (is(Type::Object)) {
			return getNoCheck<JsonObject>().size();
		}
		return 1;
	}

	void Json::dumpTo(std::string& out, std::string_view indent, u64 depth) const {
		const bool pretty = !indent.empty();

		switch (getType()) {
		case Type::Null:
			out += "null";
			break;
		case Type::Bool:
			out += std::get<bool>(storage) ? "true" : "false";
			break;
		case Type::Int:
			appendNumber(out, std::get<i64>(storage));
			break;
		case Type::UInt:
			appendNumber(out, std::get<u64>(storage));
			break;
		case Type::Float:
			appendNumber(out, std::get<double>(storage));
			break;
		case Type::String:
			appendEscapedString(out, *std::get<Unique<std::string>>(storage));
			break;
		case Type::Array: {
			const auto& array = *std::get<Unique<JsonArray>>(storage);
			out.push_back('[');
			for (u64 i = 0; i < array.size(); ++i) {
				if (i != 0) {
					out.push_back(',');
				}
				if (pretty) {
					out.push_back('\n');
					appendIndent(out, indent, depth + 1);
				}
				array[i].dumpTo(out, indent, depth + 1);
			}
			if (!array.empty() && pretty) {
				out.push_back('\n');
				appendIndent(out, indent, depth);
			}
			out.push_back(']');
			break;
		}
		case Type::Object: {
			const auto& object = *std::get<Unique<JsonObject>>(storage);
			out.push_back('{');
			bool first = true;
			for (const auto& [key, value] : object) {
				if (!first) {
					out.push_back(',');
				}
				if (pretty) {
					out.push_back('\n');
					appendIndent(out, indent, depth + 1);
				}
				appendEscapedString(out, key);
				out.push_back(':');
				if (pretty) {
					out.push_back(' ');
				}
				value.dumpTo(out, indent, depth + 1);
				first = false;
			}
			if (!object.empty() && pretty) {
				out.push_back('\n');
				appendIndent(out, indent, depth);
			}
			out.push_back('}');
			break;
		}
		}
	}

	std::string Json::dump() const {
		std::string out;
		dumpTo(out, "\t", 0);
		return out;
	}

	std::string Json::dump(u64 indentWidth) const {
		if (indentWidth > maxDumpIndentWidth) {
			logger.error_and_throw("Json dump indent width is too large: {}", indentWidth);
		}

		std::string out;
		dumpTo(out, std::string(indentWidth, ' '), 0);
		return out;
	}

	void Json::dumpToFile(const std::filesystem::path& file) const {
		std::ofstream out(file, std::ios::binary | std::ios::trunc);
		if (!out) {
			logger.error_and_throw("Failed to open json file '{}' for writing", file.string());
		}

		out << dump();
		if (!out) {
			logger.error_and_throw("Failed to write json file '{}'", file.string());
		}
	}

	void Json::dumpToFile(const std::filesystem::path& file, u64 indentWidth) const {
		std::ofstream out(file, std::ios::binary | std::ios::trunc);
		if (!out) {
			logger.error_and_throw("Failed to open json file '{}' for writing", file.string());
		}

		out << dump(indentWidth);
		if (!out) {
			logger.error_and_throw("Failed to write json file '{}'", file.string());
		}
	}

} // namespace flx::json
