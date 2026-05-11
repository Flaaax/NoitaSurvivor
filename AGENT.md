# AGENT Instructions

- This is a migration project: the original project was a Visual Studio project, and this repository is being migrated to CMake for CLion compatibility.
- The original Visual Studio project is located at `E:\VisualC++\.SFML projects\NoitaSurvivor`.
- Prefer preserving behavior from the original project when investigating migration-related issues.
- Do not manually run compile, build, generation, or codegen commands.
- Do not invoke tools such as `cmake --build`, `make`, `ninja`, `msbuild`, Visual Studio build commands, or project-specific generator scripts unless the user explicitly asks for it.
- When changes may require generated files or a build to verify, explain what should be run instead of running it yourself.
- Keep edits focused on source/configuration files and avoid modifying generated output by hand.
