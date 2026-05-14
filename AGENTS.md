# AGENT Instructions

- This is a migration project: the original project was a Visual Studio project, and this repository is being migrated to CMake for CLion compatibility.
- The original Visual Studio project is located at `E:\VisualC++\.SFML projects\NoitaSurvivor`.
- Prefer preserving behavior from the original project when investigating migration-related issues.
- Do not manually run compile, build, generation, or codegen commands.
- Do not invoke tools such as `cmake --build`, `make`, `ninja`, `msbuild`, Visual Studio build commands, or project-specific generator scripts unless the user explicitly asks for it.
- When changes may require generated files or a build to verify, explain what should be run instead of running it yourself.
- Keep edits focused on source/configuration files and avoid modifying generated output by hand.

## Backlog

- After the Box2D 3.x migration is committed, consider removing the project's direct `fmt` dependency. Replace project-owned `fmt::format` / `fmt::runtime` usage in `Logger` and `Assert` with `std::format` / `std::vformat`, then remove direct `fmt` entries from `vcpkg.json`, `find_package`, and `target_link_libraries`. `fmt` may remain as a transitive dependency of `spdlog`.


## 备注
- 此分支正在重构，所以无法通过编译

# AGENT行为准则
- 发现用户指定任务之外的bug，接口不一致，代码语法错误等时，如果用户没指定你擅自修改，则仅报告。
- git comment 应使用中英双语，英语标题，代码注释仅使用英语。
- 如果你要执行重构，除非任务直接要求重构，否则只报告，并待用户确认。
