# AGENT Instructions

- Do not manually run compile, build, generation, or codegen commands.
- Do not invoke tools such as `cmake --build`, `make`, `ninja`, `msbuild`, Visual Studio build commands, or project-specific generator scripts unless the user explicitly asks for it.
- When changes may require generated files or a build to verify, explain what should be run instead of running it yourself.
- Keep edits focused on source/configuration files and avoid modifying generated output by hand.
