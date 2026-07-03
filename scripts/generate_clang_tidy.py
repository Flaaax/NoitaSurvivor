#!/usr/bin/env python3
import argparse
import importlib.util
import os
import sys
from pathlib import Path
from typing import Any


def load_fon_module():
	fon_root = Path(os.environ.get("FON_ROOT", r"E:\FON"))
	parser_path = fon_root / "parser" / "fon_parser.py"

	if not parser_path.is_file():
		raise RuntimeError(f"FON parser not found: {parser_path}")

	module_name = "fon_parser"
	spec = importlib.util.spec_from_file_location(module_name, parser_path)
	if spec is None or spec.loader is None:
		raise RuntimeError(f"Cannot load FON parser: {parser_path}")

	module = importlib.util.module_from_spec(spec)
	sys.modules[module_name] = module

	try:
		spec.loader.exec_module(module)
	except Exception:
		sys.modules.pop(module_name, None)
		raise

	return module


def yaml_scalar(value: Any) -> str:
	if isinstance(value, bool):
		return "true" if value else "false"
	if value is None:
		return "null"
	if isinstance(value, (int, float)):
		return str(value)

	text = str(value)
	escaped = text.replace("'", "''")
	return f"'{escaped}'"


def emit_checks(lines: list[str], value: Any) -> None:
	if not isinstance(value, list):
		raise RuntimeError("Checks must be an array in clang-tidy.fon")

	checks = []
	for item in value:
		if not isinstance(item, str):
			raise RuntimeError("Checks array must contain only strings")
		checks.append(item)

	lines.append("Checks: >")

	non_empty_indexes = [i for i, item in enumerate(checks) if item != ""]
	last_non_empty = non_empty_indexes[-1] if non_empty_indexes else -1

	for i, item in enumerate(checks):
		if item == "":
			lines.append("")
		elif i == last_non_empty:
			lines.append(f"    {item}")
		else:
			lines.append(f"    {item},")


def emit_yaml(lines: list[str], key: str, value: Any) -> None:
	if key == "Checks":
		emit_checks(lines, value)
		return

	if isinstance(value, dict):
		lines.append(f"{key}:")
		for child_key, child_value in value.items():
			lines.append(f"  {child_key}: {yaml_scalar(child_value)}")
		return

	if isinstance(value, list):
		lines.append(f"{key}:")
		for item in value:
			lines.append(f"  - {yaml_scalar(item)}")
		return

	lines.append(f"{key}: {yaml_scalar(value)}")


def generate(config: dict[str, Any]) -> str:
	if not isinstance(config, dict):
		raise RuntimeError("clang-tidy.fon root must be an object")

	lines: list[str] = []
	for key, value in config.items():
		if not isinstance(key, str):
			raise RuntimeError("clang-tidy.fon keys must be strings")

		if lines:
			lines.append("")

		emit_yaml(lines, key, value)

	return "\n".join(lines) + "\n"


def main() -> int:
	parser = argparse.ArgumentParser(
		description="Generate .clang-tidy from clang-tidy.fon."
	)
	parser.add_argument("input", nargs="?", default="scripts/clang-tidy/clang-tidy.fon")
	parser.add_argument("output", nargs="?", default=".clang-tidy")
	args = parser.parse_args()

	fon = load_fon_module()

	input_path = Path(args.input)
	output_path = Path(args.output)

	with input_path.open("r", encoding="utf-8-sig") as fp:
		config = fon.load(fp)

	text = generate(config)

	old_text = None
	if output_path.is_file():
		old_text = output_path.read_text(encoding="utf-8-sig")

	if old_text == text:
		return 0

	output_path.write_text(text, encoding="utf-8", newline="\n")
	return 0


if __name__ == "__main__":
	try:
		raise SystemExit(main())
	except Exception as exc:
		print(f"generate_clang_tidy.py: {exc}", file=sys.stderr)
		raise SystemExit(1)