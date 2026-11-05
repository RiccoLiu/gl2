# GL2 OpenGL 学习工程

这是一个基于 C++17、CMake 和 OpenGL 的图形编程学习工程。源码按 LearnOpenGL 教程章节组织，以多个相互独立的示例程序演示窗口创建、基础绘制、模型导入和实时光照技术。它目前是示例集合，不是单一的综合应用。

## 技术栈

- C++17
- OpenGL 3.3 Core Profile
- GLFW：窗口、上下文及输入事件
- GLAD：加载 OpenGL 函数指针
- GLM：向量、矩阵和变换
- Assimp：3D 模型导入
- stb_image：图像读取

这些依赖以源码或预编译二进制形式放在 `thirdparty/` 中，并由 CMake 集成。

## 目录结构

```text
.
├── CMakeLists.txt             # 顶层工程配置：依赖注册、gl_common、示例清单
├── cmake/GlExample.cmake      # add_gl_example 辅助函数（目标 + 着色器/资源/DLL 复制）
├── include/                   # 共享基础层（header-only）
│   ├── shader.h               # RAII 着色器程序：加载/编译/链接、uniform 缓存
│   ├── camera.h               # 第一人称相机与输入变换
│   ├── mesh.h                 # RAII 网格：VAO/VBO/EBO 创建与绘制
│   └── model.h                # Assimp 场景遍历、纹理去重与生命周期管理
├── resources/                 # 模型、纹理等运行资源
├── src/                       # 按教程章节划分的独立示例及着色器
├── thirdparty/                # GLFW、GLAD、GLM、Assimp、stb_image
├── note/                      # 学习笔记和辅助图片
├── build/                     # 本地 CMake 构建目录
└── bin/                       # 可执行文件、着色器、资源与 DLL 的输出目录
```

## 架构说明

工程采用“每个教程示例一个可执行目标”的组织方式。顶层 `CMakeLists.txt` 统一设置语言标准（MSVC 下自动加 `/utf-8`）、注册第三方依赖，并通过 `add_gl_example` 声明每个示例；每个示例只写一行登记，不再有各自的 CMakeLists。

共享层 `include/` 提供教程中反复使用的基础能力：

- `Shader`：构造时读入 `.vs/.fs(/.gs)` 完成编译链接，失败抛 `std::runtime_error`；析构释放 program；move-only；uniform location 按名称缓存。
- `Mesh`：构造时创建 VAO/VBO/EBO，析构释放，move-only；`draw` 绑定纹理与索引绘制；纹理句柄由 `Model` 统一拥有。
- `Model`：`std::filesystem` 处理路径（兼容 `/` 和 `\`），纹理以 `unordered_map` 去重，析构释放所有纹理；空网格、未知纹理通道数等边界情况有兜底。
- `Camera`：欧拉角 FPS 相机，接口保持与教程一致。

模型加载示例的主要数据流为：Assimp 读取模型场景 → `Model` 遍历节点和网格 → 转换为本工程的 `Vertex`、索引和纹理 → `Mesh` 创建 VAO/VBO/EBO → 着色器绘制网格。

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