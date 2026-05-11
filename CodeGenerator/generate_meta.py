#!/usr/bin/env python3
"""Generate ComponentMeta initializers from component headers.

This is a lightweight replacement for the C# CodeGenerator project. It is
intentionally narrow: it mirrors the current generator's simple token parser and
is not a general-purpose C++ parser.
"""

from __future__ import annotations

import argparse
import dataclasses
import pathlib
import re
import shutil
import subprocess
import sys
import tempfile
from typing import Iterable


INPUT_FILES = (
    "src/game/Components/EntityComponents.h",
    "src/game/Components/PhysicsComponents.h",
)
OUTPUT_FILE = "src/meta/generated/Meta_initComponentInitGen.cpp"


class StopIterationInTokens(Exception):
    pass


@dataclasses.dataclass(frozen=True)
class Token:
    name: str
    kind: str


@dataclasses.dataclass
class StructInfo:
    name: str
    fields: list[str] = dataclasses.field(default_factory=list)


TOKEN_PATTERNS: tuple[tuple[re.Pattern[str], str], ...] = (
    (re.compile(r"//.*?(?=\r?\n|\r|$)"), "Comment"),
    (re.compile(r"/\*[\s\S]*?\*/"), "Comment"),
    (re.compile(r'"(?:\\.|[^"\\\r\n])*"'), "Literal"),
    (re.compile(r"'(?:\\.|[^'\\\r\n])'"), "Literal"),
    (re.compile(r"(?:\d+|(?:\d+\.|\d*\.\d+)f)"), "Literal"),
    (re.compile(r"[a-zA-Z_][a-zA-Z0-9_]*"), "Word"),
    (re.compile(r"[=+*/&^#!{}()\[\]<>\?;:|,~\\%.\-]"), "Symbol"),
)


def tokenize(code: str) -> list[Token]:
    tokens: list[Token] = []
    index = 0
    while index < len(code):
        while index < len(code) and code[index] in " \n\r\t":
            index += 1
        if index >= len(code):
            break

        for pattern, kind in TOKEN_PATTERNS:
            match = pattern.match(code, index)
            if match:
                tokens.append(Token(match.group(0), kind))
                index = match.end()
                break
        else:
            index += 1
    return tokens


class TokenIterator:
    def __init__(self, tokens: list[Token], index: int = 0) -> None:
        self.tokens = tokens
        self.index = index

    def current(self) -> Token:
        if self.index >= len(self.tokens):
            raise StopIterationInTokens
        return self.tokens[self.index]

    def last(self) -> Token | None:
        return self.tokens[self.index - 1] if self.index >= 1 else None

    def next(self) -> None:
        self.index += 1
        if self.index >= len(self.tokens):
            raise StopIterationInTokens

    def find(self, name: str | None, kind: str | None = None) -> None:
        while True:
            token = self.current()
            if (kind is None or token.kind == kind) and (name is None or token.name == name):
                return
            self.next()

    def skip_scope(self) -> None:
        if self.current().name != "{":
            raise RuntimeError(f"skip_scope expected '{{', got {self.current().name!r}")
        depth = 1
        while depth:
            self.next()
            match self.current().name:
                case "{":
                    depth += 1
                case "}":
                    depth -= 1
        self.next()

    def skip_field(self) -> None:
        depth = 0
        while True:
            match self.current().name:
                case "{":
                    depth += 1
                case "}":
                    depth -= 1
                case ";":
                    if depth == 0:
                        self.next()
                        return
            self.next()

    def skip_function(self) -> None:
        depth = 0
        has_body = False
        while (not has_body) or depth != 0:
            match self.current().name:
                case "{":
                    has_body = True
                    depth += 1
                case "}":
                    depth -= 1
                case ";":
                    if not has_body:
                        self.next()
                        return
            self.next()


def skip_noinit_struct(iterator: TokenIterator) -> None:
    iterator.find("{")
    iterator.skip_scope()


def parse_structs(tokens: list[Token]) -> list[StructInfo]:
    structs: list[StructInfo] = []
    it = TokenIterator(tokens)

    try:
        while True:
            it.find("struct", "Word")

            if it.last() and it.last().name == "N_NOINIT":
                skip_noinit_struct(it)
                continue

            it.next()
            if it.current().kind == "Word" and it.current().name == "N_NOINIT":
                skip_noinit_struct(it)
                continue

            if it.current().name == ";":
                it.next()
                continue

            if it.current().kind != "Word":
                it.next()
                continue

            name = it.current().name
            it.next()
            if it.current().name != "{":
                it.find("{")
            it.next()

            info = StructInfo(name)
            can_visit = True

            while True:
                while it.current().kind == "Comment":
                    it.next()

                token_name = it.current().name
                if token_name == "}":
                    structs.append(info)
                    break
                if token_name == "public":
                    can_visit = True
                    it.next()
                    if it.current().name == ":":
                        it.next()
                    continue
                if token_name in ("private", "protected"):
                    can_visit = False
                    it.next()
                    if it.current().name == ":":
                        it.next()
                    continue
                if token_name == "{":
                    it.skip_scope()
                    continue
                if token_name in ("union", "struct", "class"):
                    it.find("{")
                    it.skip_scope()
                    continue

                should_skip = token_name in ("const", "volatile", "N_NOINIT") or not can_visit
                last = it.current()
                it.next()
                is_field = False

                while True:
                    token_name = it.current().name
                    if token_name == ";":
                        is_field = True
                        break
                    if token_name == "(":
                        break
                    if token_name in ("{", "="):
                        is_field = True
                        break
                    last = it.current()
                    it.next()

                if is_field:
                    if not should_skip:
                        info.fields.append(last.name)
                    it.skip_field()
                else:
                    it.skip_function()
    except StopIterationInTokens:
        return structs


def read_structs(repo_root: pathlib.Path, input_files: Iterable[str]) -> list[StructInfo]:
    structs: list[StructInfo] = []
    for input_file in input_files:
        code = (repo_root / input_file).read_text(encoding="utf-8-sig")
        structs.extend(parse_structs(tokenize(code)))
    return structs


def generate_code(structs: list[StructInfo], include_files: Iterable[str]) -> str:
    lines: list[str] = [
        '#include "../ComponentMeta.h"',
        '#include "../CustomFieldParser.h"',
        '#include "src/ecs/entity.h"',
        '#include "src/game/GameContext.h"',
    ]
    lines.extend(f'#include "{include_file}"' for include_file in include_files)
    lines.extend(
        [
            "",
            "template <class T> struct ValueWrapper {",
            "\tusing Parser = FieldParser<T>;",
            "\tstatic constexpr bool enabled = Parser::enabled;",
            "\tusing Storage = std::conditional_t<enabled, std::optional<T>, EmptyFieldType>;",
            "\tStorage storage{};",
            "",
            "\tconst T& value() const {",
            "\t\tif constexpr (enabled) {",
            "\t\t\treturn storage.value();",
            "\t\t}",
            '\t\tthrow "Not supposed to be here...";',
            "\t}",
            "};",
            "",
            "void ComponentMeta::initGeneratedComponentInitializers() {",
        ]
    )

    for info in structs:
        name = info.name
        lines.extend(
            [
                f'\tcomponentInitializerFactories["{name}"] =',
                "\t\t[](const json& jsonData) -> ComponentInitializer {",
            ]
        )
        for field in info.fields:
            lines.append(f"\t\tusing __{field}_t = ValueWrapper<decltype({name}::{field})>;")
        lines.append(f"\t\tstruct __{name}Parser {{")
        for field in info.fields:
            lines.append(f"\t\t\t__{field}_t {field}{{}};")
        lines.extend(["\t\t} p;", ""])

        for field in info.fields:
            lines.extend(
                [
                    f"\t\tif constexpr (__{field}_t::enabled) {{",
                    f'\t\t\tif (jsonData.contains("{field}")) {{',
                    f'\t\t\t\tp.{field}.storage = std::move(__{field}_t::Parser::parse(jsonData["{field}"]));',
                    "\t\t\t}",
                    "\t\t}",
                ]
            )
        lines.extend(
            [
                "",
                "\t\treturn [p = std::move(p)](GameCtx& ctx, myecs::entity e) -> void {",
                f"\t\t\tauto& c = ctx.reg.emplace<{name}>(e);",
                "",
            ]
        )
        for field in info.fields:
            lines.extend(
                [
                    f"\t\t\tif (p.{field}.storage) {{",
                    f"\t\t\t\tc.{field} = p.{field}.value();",
                    "\t\t\t}",
                ]
            )
        lines.extend(["\t\t};", "\t};", ""])

    lines.append("}")
    return "\n".join(lines) + "\n"


def maybe_clang_format(code: str, clang_format: str | None, repo_root: pathlib.Path) -> str:
    if not clang_format:
        return code

    # Keep the formatting probe in the repository root so clang-format resolves
    # the project-level .clang-format instead of src/meta/generated/.clang-format.
    with tempfile.NamedTemporaryFile(
        "w+",
        prefix=".generate_meta_",
        suffix=".cpp",
        dir=repo_root,
        delete=False,
        encoding="utf-8",
    ) as temp:
        temp_path = pathlib.Path(temp.name)
        temp.write(code)

    try:
        subprocess.run([clang_format, "-i", str(temp_path)], check=True)
        return temp_path.read_text(encoding="utf-8")
    finally:
        temp_path.unlink(missing_ok=True)


def update_file(path: pathlib.Path, code: str, check_only: bool) -> bool:
    old_code = path.read_text(encoding="utf-8") if path.exists() else None
    if old_code == code:
        print("Generated file is up to date.")
        return False

    if check_only:
        print(f"Generated file is out of date: {path}")
        return True

    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(code, encoding="utf-8")
    print(f"Updated generated file: {path}")
    return True


def main(argv: list[str]) -> int:
    script_dir = pathlib.Path(__file__).resolve().parent
    default_repo_root = script_dir.parent

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo-root", type=pathlib.Path, default=default_repo_root)
    parser.add_argument("--output", type=pathlib.Path, default=None)
    parser.add_argument("--check", action="store_true", help="Report stale output without writing.")
    parser.add_argument("--no-format", action="store_true", help="Do not run clang-format even if present.")
    args = parser.parse_args(argv)

    repo_root = args.repo_root.resolve()
    output = args.output if args.output else repo_root / OUTPUT_FILE
    if not output.is_absolute():
        output = repo_root / output

    structs = read_structs(repo_root, INPUT_FILES)
    code = generate_code(structs, INPUT_FILES)
    clang_format = None if args.no_format else shutil.which("clang-format")
    code = maybe_clang_format(code, clang_format, repo_root)
    changed = update_file(output, code, args.check)
    return 1 if args.check and changed else 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
