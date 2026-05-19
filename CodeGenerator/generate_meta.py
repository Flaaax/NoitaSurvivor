#!/usr/bin/env python3
# Generates component initializer and meta-info code.
# Usage:
#   python CodeGenerator/generate_meta.py
#   python CodeGenerator/generate_meta.py test

import os
import re
import sys
import tempfile
import subprocess

INPUT_FILES = [
	"src/game/Components/EntityComponents.h",
	"src/game/Components/PhysicsComponents.h",
]

OUTPUT_FILE = "src/meta/generated/Meta_initComponentInitGen.cpp"


class Token:
	def __init__(self, text, kind):
		self.text = text
		self.kind = kind


class StructInfo:
	def __init__(self, name):
		self.name = name
		self.fields = []


class TokenReader:
	def __init__(self, tokens):
		self.tokens = tokens
		self.index = 0

	def current(self):
		if self.index >= len(self.tokens):
			return None
		return self.tokens[self.index]

	def previous(self):
		if self.index <= 0:
			return None
		return self.tokens[self.index - 1]

	def next(self):
		self.index += 1

	def find(self, text, kind):
		while self.current() is not None:
			token = self.current()
			text_ok = text is None or token.text == text
			kind_ok = kind is None or token.kind == kind
			if text_ok and kind_ok:
				return True
			self.next()
		return False

	def skip_brace_scope(self):
		# Skip nested { ... } blocks.
		if self.current() is None or self.current().text != "{":
			return

		depth = 1
		self.next()

		while self.current() is not None and depth > 0:
			text = self.current().text
			if text == "{":
				depth += 1
			elif text == "}":
				depth -= 1
			self.next()

	def skip_field(self):
		# Field initializers may contain braces, for example: Vec2 value{};
		depth = 0

		while self.current() is not None:
			text = self.current().text

			if text == "{":
				depth += 1
			elif text == "}":
				if depth > 0:
					depth -= 1
			elif text == ";" and depth == 0:
				self.next()
				return

			self.next()

	def skip_function(self):
		has_body = False
		depth = 0

		while self.current() is not None:
			text = self.current().text

			if text == "{":
				has_body = True
				depth += 1
			elif text == "}":
				depth -= 1
				if has_body and depth <= 0:
					self.next()
					return
			elif text == ";" and not has_body:
				self.next()
				return

			self.next()


def read_file(path):
	file = open(path, "r", encoding="utf-8-sig")
	try:
		return file.read()
	finally:
		file.close()


def write_file(path, text):
	folder = os.path.dirname(path)
	if folder and not os.path.isdir(folder):
		os.makedirs(folder)

	file = open(path, "w", encoding="utf-8", newline="\n")
	try:
		file.write(text)
	finally:
		file.close()


def tokenize(code):
	patterns = [
		("Comment", re.compile(r"//.*?(?=\r?\n|\r|$)")),
		("Comment", re.compile(r"/\*[\s\S]*?\*/")),
		("Literal", re.compile(r'"(?:\\.|[^"\\\r\n])*"')),
		("Literal", re.compile(r"'(?:\\.|[^'\\\r\n])'")),
		("Literal", re.compile(r"(?:\d+\.\d*|\d*\.\d+|\d+)(?:f)?")),
		("Word", re.compile(r"[a-zA-Z_][a-zA-Z0-9_]*")),
		("Symbol", re.compile(r"[=+*/&^#!{}()\[\]<>\?;:|,~\\%.\-]")),
	]

	tokens = []
	index = 0

	while index < len(code):
		if code[index] in " \t\r\n":
			index += 1
			continue

		matched = False

		for item in patterns:
			kind = item[0]
			pattern = item[1]
			match = pattern.match(code, index)

			if match is not None:
				tokens.append(Token(match.group(0), kind))
				index = match.end()
				matched = True
				break

		if not matched:
			index += 1

	return tokens


def skip_noinit_struct(reader):
	if reader.find("{", None):
		reader.skip_brace_scope()


def parse_structs(tokens):
	structs = []
	reader = TokenReader(tokens)

	while reader.find("struct", "Word"):
		previous = reader.previous()
		reader.next()

		if previous is not None and previous.text == "N_NOINIT":
			skip_noinit_struct(reader)
			continue

		token = reader.current()
		if token is None:
			break

		if token.kind == "Word" and token.text == "N_NOINIT":
			skip_noinit_struct(reader)
			continue

		if token.text == ";":
			reader.next()
			continue

		if token.kind != "Word":
			reader.next()
			continue

		struct_name = token.text

		if not reader.find("{", None):
			break

		info = StructInfo(struct_name)
		reader.next()
		can_visit = True

		while reader.current() is not None:
			while reader.current() is not None and reader.current().kind == "Comment":
				reader.next()

			token = reader.current()
			if token is None:
				break

			text = token.text

			if text == "}":
				structs.append(info)
				reader.next()
				break

			if text == "public":
				can_visit = True
				reader.next()
				if reader.current() is not None and reader.current().text == ":":
					reader.next()
				continue

			if text == "private" or text == "protected":
				can_visit = False
				reader.next()
				if reader.current() is not None and reader.current().text == ":":
					reader.next()
				continue

			if text == "{":
				reader.skip_brace_scope()
				continue

			if text == "union" or text == "struct" or text == "class" or text == "enum":
				if reader.find("{", None):
					reader.skip_brace_scope()
				continue

			skip_this_field = False
			if not can_visit:
				skip_this_field = True
			if text == "const" or text == "volatile" or text == "N_NOINIT":
				skip_this_field = True

			last_word = None
			if token.kind == "Word":
				last_word = token.text

			reader.next()
			is_field = False

			while reader.current() is not None:
				token = reader.current()
				text = token.text

				if text == ";":
					is_field = True
					break
				if text == "(":
					break
				if text == "{" or text == "=":
					is_field = True
					break
				if token.kind == "Word":
					last_word = token.text

				reader.next()

			if is_field:
				if not skip_this_field and last_word is not None:
					info.fields.append(last_word)
				reader.skip_field()
			else:
				reader.skip_function()

	return structs


def collect_structs(repo_root):
	structs = []

	for input_file in INPUT_FILES:
		path = os.path.join(repo_root, input_file)
		code = read_file(path)
		tokens = tokenize(code)
		parsed = parse_structs(tokens)

		for info in parsed:
			structs.append(info)

	return structs


def add(lines, text):
	lines.append(text)


def generate_header(lines):
	add(lines, "// This file is generated by CodeGenerator/generate_meta.py.")
	add(lines, "// It registers JSON component initializers and generated component meta information.")
	add(lines, "// Do not edit this file by hand.")
	add(lines, "")
	add(lines, '#include "src/meta/MetaHeader.h"')

	for input_file in INPUT_FILES:
		add(lines, '#include "' + input_file + '"')

	add(lines, "")


def generate_component_initializers(lines, structs):
	add(lines, "void ComponentMeta::initGeneratedComponentInitializers() {")

	for info in structs:
		add(lines, 'componentInitializerFactories["' + info.name + '"] =')
		add(lines, "[](const json& jsonData) -> ComponentInitializer {")

		for field in info.fields:
			add(lines, "using __" + field + "_t = ValueWrapper<decltype(" + info.name + "::" + field + ")>;")

		add(lines, "struct __" + info.name + "Parser {")
		for field in info.fields:
			add(lines, "__" + field + "_t " + field + "{};")
		add(lines, "} p;")

		for field in info.fields:
			add(lines, "if constexpr (__" + field + "_t::enabled) {")
			add(lines, 'if (jsonData.contains("' + field + '")) {')
			add(lines, 'p.' + field + '.storage = std::move(__' + field + '_t::Parser::parse(jsonData["' + field + '"]));')
			add(lines, "}")
			add(lines, "}")

		add(lines, "return [p = std::move(p)](const GameCtx& ctx, myecs::entity e) -> void {")
		add(lines, "auto& c = ctx.reg.emplace<" + info.name + ">(e);")

		for field in info.fields:
			add(lines, "if (p." + field + ".storage) {")
			add(lines, "c." + field + " = p." + field + ".value();")
			add(lines, "}")

		add(lines, "};")
		add(lines, "};")
		add(lines, "")

	add(lines, "}")
	add(lines, "")


def generate_meta_info(lines, structs):
	add(lines, "void ComponentMeta::initGeneratedMetaInfo() {")

	for info in structs:
		add(lines, "{")
		add(lines, 'auto& info = componentMetaInfo["' + info.name + '"];')

		for field in info.fields:
			field_type = "decltype(" + info.name + "::" + field + ")"
			alias = "__" + info.name + "_" + field + "_t"

			add(lines, "using " + alias + " = ValueWrapper<" + field_type + ">;")
			add(lines, "if constexpr (" + alias + "::enabled) {")
			add(lines, 'info.fields.emplace_back(Field{"' + field + '", Util::typeFullName<' + field_type + ">()});")
			add(lines, "}")

		add(lines, "}")
		add(lines, "")

	add(lines, "}")


def generate_code(structs):
	lines = []
	generate_header(lines)
	generate_component_initializers(lines, structs)
	generate_meta_info(lines, structs)
	return "\n".join(lines) + "\n"


def clang_format(repo_root, code):
	# Temp file is placed in repo root so clang-format uses the project .clang-format.
	temp = tempfile.NamedTemporaryFile(
		mode="w",
		encoding="utf-8",
		newline="\n",
		suffix=".cpp",
		prefix=".generate_meta_",
		dir=repo_root,
		delete=False,
	)

	temp_path = temp.name

	try:
		temp.write(code)
		temp.close()

		try:
			result = subprocess.run(
				["clang-format", "-i", temp_path],
				stdout=subprocess.PIPE,
				stderr=subprocess.PIPE,
				text=True,
			)
		except FileNotFoundError:
			raise RuntimeError("clang-format was not found in PATH.")

		if result.returncode != 0:
			raise RuntimeError("clang-format failed:\n" + result.stderr)

		return read_file(temp_path)
	finally:
		if not temp.closed:
			temp.close()
		if os.path.exists(temp_path):
			os.remove(temp_path)


def main():
	print("Launching code generator...")
	script_dir = os.path.dirname(os.path.abspath(__file__))
	repo_root = os.path.abspath(os.path.join(script_dir, ".."))
	output_path = os.path.join(repo_root, OUTPUT_FILE)

	test_mode = False
	if len(sys.argv) > 1 and sys.argv[1] == "test":
		test_mode = True

	structs = collect_structs(repo_root)
	code = generate_code(structs)
	code = clang_format(repo_root, code)

	if test_mode:
		test_path = output_path + ".test"
		write_file(test_path, code)
		print("Generated test file: " + test_path)
		return 0

	old_code = None
	if os.path.isfile(output_path):
		old_code = read_file(output_path)

	if old_code == code:
		print("Generated file is up to date.")
		return 0

	write_file(output_path, code)
	print("Updated generated file: " + output_path)
	return 0


if __name__ == "__main__":
	sys.exit(main())
