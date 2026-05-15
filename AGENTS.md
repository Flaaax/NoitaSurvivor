# AGENT Instructions

- This project was migrated from a Visual Studio project to CMake for CLion compatibility. The main migration/refactor work is now complete, and the project is expected to compile and run.
- The original Visual Studio project is located at `E:\VisualC++\.SFML projects\NoitaSurvivor`.
- Prefer preserving behavior from the original project when investigating behavior regressions or migration-related issues.
- Do not manually run compile, build, generation, or codegen commands.
- Do not invoke tools such as `cmake --build`, `make`, `ninja`, `msbuild`, Visual Studio build commands, or project-specific generator scripts unless the user explicitly asks for it.
- When changes may require generated files or a build to verify, explain what should be run instead of running it yourself.
- Keep edits focused on source/configuration files and avoid modifying generated output by hand.

## Backlog

- Prepare the next major refactor: upgrade SFML from 2.x to 3.x and upgrade ImGui-SFML in the same migration. 
- Keep these changes coordinated because window/event/rendering APIs and ImGui integration are coupled.


## 备注
- 项目box2d 2.x->3.x 迁移/重构已基本完成，目前可编译运行，核心玩法已基本还原。
- 部分非核心功能可能仍处于临时禁用或待恢复状态，例如 imgui。

# AGENT行为准则
- 发现用户指定任务之外的bug，接口不一致，代码语法错误等时，如果用户没指定你擅自修改，则仅报告。
- git comment 应使用中英双语，英语标题，代码注释仅使用英语。
- 严禁重构。除非，用户的要求中说“为我直接执行重构”。如果用户仅提到“重构”也不算。
