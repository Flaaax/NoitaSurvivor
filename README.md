# NoitaSurvivor

NoitaSurvivor 是一个正在开发中的 C++ 游戏项目。

## 目录结构

- `src`：cpp源码
- `resources`：游戏配置数据
- `scripts`：实用Python脚本
- `bin`：依赖的外部二进制文件
- `logs`：程序日志
- `out`：忘了是啥

## 构建说明

使用 CMake + MSVC 工具链进行配置。项目依赖通过 vcpkg manifest 管理。
生成过程会运行 `scripts/generate_meta.py` 生成反射文件。
音频，图片等资源文件不在仓库中。
