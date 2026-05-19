# NoitaSurvivor

NoitaSurvivor 是一个正在开发中的 C++ 生存类游戏项目。项目已从 Visual Studio 工程迁移到 CMake，主要用于 CLion / MSVC 环境下继续开发。

## 当前状态

项目仍在构建中，核心玩法已基本还原，目前主要处于 UI 系统、事件分发机制和调试工具链的重构阶段。

## 技术栈

- C++20
- CMake 3.26+
- MSVC
- vcpkg manifest
- SFML 3
- Box2D 3
- ImGui-SFML
- fmt / spdlog / nlohmann-json

## 目录结构

- `src/`：主要源码
- `src/ecs/`：自研ECS引擎
- `resources/`：游戏资源，包括数据、字体、图像和音效
- `CodeGenerator/`：构建前使用的元数据生成脚本
- `CMakeLists.txt`：CMake 工程配置
- `vcpkg.json`：依赖声明与版本约束

## 构建说明

使用支持 CMake 的 IDE 打开项目，并使用 MSVC 工具链进行配置。项目依赖通过 vcpkg manifest 管理，目标程序为 `NoitaSurvivor`。
构建流程中会运行 `CodeGenerator/generate_meta.py` 生成必要的元数据文件。
程序运行需要额外资源文件（不在仓库中）。