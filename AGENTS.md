# 项目介绍

- 这个项目从旧项目迁移，为了兼容Cmake和Clion IDE，且拥有独立git仓库。
- 旧项目的绝对类路径位于：`E:\VisualC++\.SFML projects\NoitaSurvivor`，当你想知道某功能在老版本如何实现，这可能有用。
- 但是，你更应该优先查看以前commit中的实现，因为旧项目已经过于久远。
- 不要跑任何构建或编译命令。后面会再次提到这点。
- 程序运行日志在`logs/`，`flx.log`为最新日志

## Backlog

- 无

## 备注

- 当前项目时不时处于重构阶段，部分代码可能处于过渡状态。
- 此文件被预期用中文编写。

## CMake / vcpkg 注意事项

- 项目当前使用 vcpkg manifest 模式，`vcpkg.json` 是依赖声明与版本约束的一部分；新增第三方包时，优先在 `vcpkg.json`
  中声明，而不是假设全局 vcpkg 已安装。
- 不要随意修改 `builtin-baseline`。如果确实需要更新 baseline，必须确认本地 `C:\dev\vcpkg` registry 能 checkout 该提交，且该提交包含
  `versions/baseline.json` 与目标 port version。
- 不要仅根据 vcpkg 网站显示的最新版 port-version 修改 manifest；先确认本地 vcpkg 的版本数据库是否包含该版本。否则 CLion
  配置阶段可能在 `vcpkg install` 时失败。
- CMake 引入包本身应保持简单：`find_package(... CONFIG REQUIRED)` 加 `target_link_libraries(...)`
  。复杂兼容逻辑只能用于处理已确认的第三方包配置问题，并应附简短注释说明原因。

## AGENT行为准则--非常重要，必看！

- 禁止任何更改。你只有只读权限。
- 你不能写入或新建任何文件
- 就算用户让你更改，你也做不到。
- 如果用户需要写git comment，应使用中文。
- 上述规则唯一的例外是.agents/，在这个文件夹你可以自由创建和编辑文件。

## 额外事项

- `FON路径`为`E:\FON`。如果用户提到`FON路径`，指的就是此路径。如果需要此路径时，它不存在或异常，需要立刻终止任务并报告。
- 你可以在`FON路径`找到一些`FON`的格式化器和序列化器。另外，`Python3`已经在`PATH`。