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
- 在此之后：重构UI系统与系统事件分发机制


## 备注
- 项目box2d 2.x->3.x 迁移/重构已基本完成，目前可编译运行，核心玩法已基本还原。
- 部分非核心功能可能仍处于临时禁用或待恢复状态，例如 imgui。

## CMake / vcpkg 注意事项
- 项目当前使用 vcpkg manifest 模式，`vcpkg.json` 是依赖声明与版本约束的一部分；新增第三方包时，优先在 `vcpkg.json` 中声明，而不是假设全局 vcpkg 已安装。
- 不要随意修改 `builtin-baseline`。如果确实需要更新 baseline，必须确认本地 `C:\dev\vcpkg` registry 能 checkout 该提交，且该提交包含 `versions/baseline.json` 与目标 port version。
- 不要仅根据 vcpkg 网站显示的最新版 port-version 修改 manifest；先确认本地 vcpkg 的版本数据库是否包含该版本。否则 CLion 配置阶段可能在 `vcpkg install` 时失败。
- 本项目当前锁定 `SFML 3.0.2`。集成 ImGui-SFML 时，要确认 ImGui-SFML 的 CMake config 没有与 SFML 3 target 命名冲突；旧 config 可能仍引用 `sfml-graphics`、`sfml-window`、`sfml-system` 这类 SFML 2 风格 target。
- CMake 引入包本身应保持简单：`find_package(... CONFIG REQUIRED)` 加 `target_link_libraries(...)`。复杂兼容逻辑只能用于处理已确认的第三方包配置问题，并应附简短注释说明原因。

# AGENT行为准则
- 发现用户指定任务之外的bug，接口不一致，代码语法错误等时，如果用户没指定你擅自修改，则仅报告。
- git comment 应使用中英双语，英语标题，代码注释仅使用英语。

## 非常重要
- 当你决定执行代码重构时，需谨慎，因为用户的发言比较随意，他可能不是真的想让你直接重构，而是让你评估是否执行重构。
- 用户偏好先审核再代码重构。
