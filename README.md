# GL2 OpenGL 学习工程

这是一个基于 C++17、CMake 和 OpenGL 的图形编程学习工程。源码按 LearnOpenGL 教程章节组织，以多个相互独立的示例程序演示窗口创建、基础绘制、模型导入和实时光照技术。它目前是示例集合，不是单一的综合应用。

## 构建与运行

需要支持 OpenGL 3.3 的显卡/驱动、CMake 3.16+ 和 C++17 编译器。Windows 下在项目根目录执行：

```powershell
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Debug
```

或在 VS 开发者命令行中使用 Ninja：

```powershell
cmake -S . -B build -G Ninja
cmake --build build
```

!! thirdparty 目录下的预编译库默认使用 vs 2022 工具链编译，如果使用其他工具链，需要自行更换预编译库。
