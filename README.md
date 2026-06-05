# NoitaSurvivor

NoitaSurvivor 是一个正在开发中的 C++ 类幸存者游戏项目。

## 目录结构

- `src/`：主要源码
- `src/ecs/`：ECS引擎
- `resources/`：游戏资源
- `CodeGenerator/`：用于反射的代码生成脚本

## 构建说明

使用 CMake + MSVC 工具链进行配置。项目依赖通过 vcpkg manifest 管理。目标程序为 `NoitaSurvivor`。
会自动运行 `CodeGenerator/generate_meta.py` 生成元文件。
音频，图片等资源文件不在仓库中。
