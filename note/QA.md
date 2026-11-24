# OpenGL 笔试面试资料整理

> 整理日期：2026-11-24
> 检索范围：2025–2026 年公开面经、技术博客、GitHub 面试资料，以及长期维护的官方文档/教程
> 适用方向：图形渲染开发、游戏引擎、图形学实习生、技术美术（TA）、音视频渲染、OpenGL ES 客户端/嵌入式开发

> 说明：本文中的“高频”是根据近期公开题目和教程覆盖范围归纳，并非严格的全行业考频统计。答案为整理总结，不是原文摘录；关键 API 行为以 Khronos 官方文档和当前使用版本为准。

---

## 目录

1. [近期资料导航](#一近期资料导航)
2. [笔试考点分布与题型](#二笔试考点分布与题型)
3. [OpenGL 高频面试题与参考答案](#三opengl-高频面试题与参考答案)
4. [常考数学与光照公式](#四常考数学与光照公式)
5. [手写代码题与代码骨架](#五手写代码题与代码骨架)
6. [OpenGL ES / 移动端专题](#六opengl-es--移动端专题)
7. [性能优化与故障排查](#七性能优化与故障排查)
8. [复习路线与项目准备](#八复习路线与项目准备)
9. [参考资料](#九参考资料)

---

## 一、近期资料导航

| 资料 | 内容与用法 | 时间信息 |
|---|---|---|
| [AuroraShader：图形学面试学习路线](https://github.com/GloryKazimierz/AuroraShader#graphics-interview-study-path--图形学面试学习路线) | GitHub 仓库，含中英文课程：管线、坐标空间、光栅化/深度、法线/切线空间、纹理、颜色/HDR、前向/延迟、PBR、阴影、GPU 性能、调试 | 检索时仓库活跃维护 |
| [115 道图形学面试题库（中文）](https://github.com/GloryKazimierz/AuroraShader/blob/main/docs/interview/12_GRAPHICS_INTERVIEW_QUESTION_BANK.zh-CN.md) | 最适合直接刷题的中英文题库；每章还把“直觉→数学→常见错误→项目实现→面试问法”串起来 | 当前仓库版本 |
| [图形学面试题总结](https://blog.csdn.net/qq_46348003/article/details/146211115) | 中文高频问答：管线、VAO/VBO/EBO、纹理、FBO、Instancing、深度/透明、Shadow Map、PBR、前向/延迟渲染 | 2025-03 |
| [游戏研发笔试备考：从 C++ 到图形学](https://blog.csdn.net/weixin_29007243/article/details/167731114) | 近期笔试复盘，强调 MVP 顺序、法线变换逆转置、光栅化、深度缓冲、光照模型 | 2026-10 |
| [搜狐畅游技术美术校招笔试复盘](https://blog.csdn.net/weixin_34268169/article/details/92110339) | 图形学/Shader 考点：坐标空间、Blinn-Phong、Shadow Map、DrawCall、移动端优化、Bug 分析 | 2026-08 更新 |
| [OpenGL 面试核心：坐标变换、渲染管线与动画数学](https://blog.csdn.net/weixin_36300623/article/details/170404840) | 从“模型为什么跑偏”讲 MVP、管线和性能，适合建立整体框架 | 2026-11 |
| [几道常见的 OpenGL 面试题](https://cloud.tencent.com/developer/article/2418145) | 纹理采样等经典问题的代码与图解，适合补基础细节 | 2024-05 |
| [Android OpenGL ES 实战：Canvas 到 GPU 渲染](https://bbs.csdn.net/weixin_29193259/article/details/100712994) | 近期移动端实战：坐标映射、FBO、滤镜、黑屏排查、纹理翻转 | 2026-10 |
| [OpenGL ES 嵌入式渲染核心原理与实践](https://blog.csdn.net/qq_40882017/article/details/155279450) | EGL、GLSL ES、版本差异、纹理压缩、移动端性能优化 | 2025-11 |
| [OpenGL 上下文、线程与共享上下文](https://m.blog.csdn.net/Kennethdroid/category_9120765.html) | 字节流动相关文章分类，包含上下文、线程、几何着色器等进阶主题 | 2025 年多篇 |
| [LearnOpenGL CN](https://learnopengl-cn.github.io/) | 最推荐的中文系统教程：现代 Core-profile，覆盖 VAO、纹理、变换、FBO、阴影、延迟着色、PBR | 长期维护 |
| [LearnOpenGL 配套代码](https://github.com/JoeyDeVries/LearnOpenGL) | 教程源码，适合按章节运行、改参数、观察现象 | 长期维护 |
| [prographon/graphics-developer-roadmap](https://github.com/prographon/graphics-developer-roadmap) | 图形开发学习路径和资料导航，含 OpenGL/Vulkan 教程、图形程序员面试清单 | 长期维护 |
| [Khronos OpenGL Wiki：渲染管线](https://www.khronos.org/opengl/wiki/Rendering_Pipeline_Overview) | 核对固定阶段、可编程阶段、裁剪/光栅化/逐片元操作的权威资料 | 官方文档 |
| [docs.gl](https://docs.gl/) | 按 OpenGL 版本查询函数签名和行为，适合确认 API 细节 | 官方文档镜像 |
| [tinyrenderer](https://github.com/ssloy/tinyrenderer) | 用少量代码手写软光栅器，能直观理解三角形、深度缓冲、纹理和投影“黑盒内部” | 经典项目 |

---

## 二、笔试考点分布与题型

### 2.1 常见题型

1. **概念选择题/判断题**：渲染管线、状态机、深度/混合顺序、纹理环绕与过滤、Core/Compatibility Profile。
2. **矩阵与坐标计算**：模型/视图/投影矩阵乘法顺序、平移旋转缩放组合、透视除法、法线变换。
3. **代码补全**：VBO/VAO/EBO 初始化、纹理上传与采样器绑定、FBO 创建与完整性检查、Uniform 更新。
4. **GLSL 阅读/改错**：`attribute/uniform/varying` 与新版 `in/out`、精度限定符、纹理坐标、输出颜色。
5. **程序输出/图像分析题**：绕序与面剔除、透明排序、深度冲突、纹理翻转、矩阵列主序。
6. **开放题**：如何实现阴影、延迟渲染、Bloom、SSAO；如何优化 DrawCall、Overdraw 和带宽。
7. **现场编码**：MVP 矩阵类、简版软光栅器、纹理滤镜、后处理、粒子/Instancing 示例。

### 2.2 优先级建议

**必会主干**：渲染管线 → MVP/坐标空间 → VAO/VBO/EBO → 纹理 → 深度/模板/混合/剔除 → FBO → GLSL → 前向/延迟 → 基础阴影。

**拉开差距**：逆置矩阵与法线、透视除法与深度精度、Early-Z、MSAA、Gamma/HDR/Tone Mapping、BRDF/PBR、GPU 性能分析、OpenGL/Vulkan 同步与线程模型。

---

## 三、OpenGL 高频面试题与参考答案

### 3.1 基础与渲染管线

**Q1：OpenGL 是什么？和 DirectX/Vulkan 有什么区别？**

OpenGL 是跨平台图形 API 规范，核心是状态机：设置当前 Program、VAO、纹理、视口、测试开关等，再发起绘制。它不负责创建窗口/处理输入（通常由 GLFW/SDL 等负责）。

- OpenGL：驱动管理大量隐式状态与资源同步，开发直接；CPU 可控制粒度低，多线程能力受上下文限制。
- Vulkan/Modern D3D12：显式管理内存、命令缓冲、同步与管线对象，支持多线程并行录制；控制力强但复杂度和代码量大。
- 不要简单说“OpenGL 不支持多线程”：它支持共享上下文等机制，但 API 对象通常不是可随意跨线程并发使用的普通 C++ 对象，使用成本高。

**Q2：描述现代 OpenGL 渲染管线。**

一条常用主线：

```text
CPU 顶点数据
  → 顶点着色器（每个顶点）
  → [细分着色器]
  → [几何着色器]
  → 图元装配
  → 裁剪（视锥体裁剪 + 齐次裁剪）
  → 屏幕映射（NDC → 窗口坐标）
  → 光栅化（生成片元并插值 varying）
  → 片元着色器（每个片元）
  → 逐片元操作：裁剪测试/抖动、模板测试、深度测试、混合、写入帧缓冲
```

要点：顶点着色器和片元着色器是最常用的可编程阶段；Geometry Shader 位于顶点着色器之后、光栅化之前，具体输入输出接口与细分等阶段按 OpenGL 版本规范描述。面试回答时先说“固定阶段 + 可编程阶段”，别背成所有阶段都可写代码。

**Q3：顶点（Vertex）、图元（Primitive）、片元（Fragment）、像素（Pixel）的区别？**

- Vertex：顶点输入及其属性。
- Primitive：点/线/三角形等几何图元。
- Fragment：光栅化产生的候选着色单元，不一定最终写屏。
- Pixel：帧缓冲中的实际采样点/最终图像单元；一个像素可能对应多个 MSAA 样本。

**Q4：Core Profile 和 Compatibility Profile 的区别？**

- Core：只保留现代可编程管线所需功能，废弃立即模式（`glBegin/glEnd`）、固定函数顶点/片元处理等。
- Compatibility：同时支持旧固定功能 API 与现代 API。
- 新项目应选 Core Profile（例如 3.3 Core 或更高，需按目标平台确认 macOS 通常支持到 4.1）；面试要避免写 `glBegin` 作为现代 OpenGL 示例。

**Q5：OpenGL 为什么叫状态机？常见状态有哪些？**

绘制结果取决于当前上下文状态，而不是 draw 调用单独决定。常见状态：当前 Program、VAO、绑定的 VBO/EBO、纹理单元与采样器、视口/裁剪框、深度/模板/混合/面剔除开关及参数、FBO、着色器 Uniform 值。需要养成“设置状态→绘制→恢复/重设”的习惯。

**Q6：OpenGL 上下文（Context）是什么？**

保存当前 OpenGL 状态和资源的容器。一个线程同一时刻一般只能有一个当前上下文；切换上下文可能触发状态切换和同步。窗口系统负责创建上下文，GLFW/SDL/EGL 等提供封装。

### 3.2 缓冲对象与绘制

**Q7：VAO、VBO、EBO 分别是什么？关系是什么？（几乎必问）**

- VBO：GPU 中的顶点属性数据（位置、法线、UV、颜色等）。
- EBO/IBO：索引数据，指定顶点复用和绘制顺序。
- VAO：记录顶点属性如何从 VBO 读取，以及顶点属性指针、启用状态、EBO 绑定等。它不复制 VBO 数据。

流程：创建 VAO → 绑定 VAO → 创建/绑定 VBO 并上传数据 → 创建/绑定 EBO 并上传索引 → `glVertexAttribPointer` + `glEnableVertexAttribArray` → 解绑 VAO。绘制时绑定 VAO，调用 draw。

**Q8：`glDrawArrays` 与 `glDrawElements` 的区别？**

- `glDrawArrays` 按顶点数组顺序绘制，不使用索引。
- `glDrawElements` 根据 EBO 中索引取顶点，可复用共享顶点、减少显存和顶点处理。
- 选择依据是顶点共享程度；立方体若每面复制 4 个顶点便于独立法线/UV，则 24 个顶点 + 36 个索引；若顶点属性允许共享，可用 8 个顶点 + 36 个索引。

**Q9：顶点属性指针 `glVertexAttribPointer` 参数怎么理解？**

依次是属性 location、每个顶点的分量数、数据类型、是否归一化、stride（相邻顶点步长）、offset（该属性在顶点内偏移）。例如交错数据 `[px,py,pz, u,v]`，位置用 3 个 float、stride `5*sizeof(float)`、offset 0；UV 用 2 个 float、offset `3*sizeof(float)`。

**Q10：什么是 Draw Call？为什么减少 Draw Call？**

一次把一组图元提交给 GPU 的绘制调用。每个 Draw Call 都有 CPU 端状态检查、命令提交和驱动开销。大量小物体时 CPU 容易成为瓶颈。优化：Instancing、合批（材质/纹理一致）、纹理图集、静态合 mesh、降低 Pass 数量。但不能盲目合批：跨对象材质不同、遮挡关系或阴影需求不同时应保留必要拆分。

### 3.3 着色器与 GLSL

**Q11：Shader 编译和链接流程？常见错误怎么排查？**

```text
glCreateShader → glShaderSource → glCompileShader
  → 检查 GL_COMPILE_STATUS，失败时获取 glGetShaderInfoLog
glCreateProgram → glAttachShader → glLinkProgram
  → 检查 GL_LINK_STATUS，失败时获取 glGetProgramInfoLog
glUseProgram → 查询 attribute/uniform location → 上传 uniform → draw
```

高频错误：`#version` 与 Profile 不匹配、输入/输出变量不匹配、location 为 -1（未使用被优化）、Uniform 未赋值、纹理单元未绑定。链接错误不一定是单个 Shader 编译错误，要看两个阶段的 interface 是否一致。

**Q12：`attribute`、`uniform`、`varying` 分别是什么？现代写法？**

- attribute（旧）：逐顶点输入，如位置、UV。
- uniform：一次绘制内相对不变的数据，如 MVP、材质参数、光源位置。
- varying（旧）：从顶点着色器传给片元着色器，光栅化时插值。
- 新版 Core Profile：顶点输入用 `in`，顶点输出/片元输入用 `out`，片元最终颜色也用 `out`；`texture()` 替代旧 `texture2D()`。版本/精度仍按目标 GLSL 版本写。

**Q13：为什么 Uniform 的 location 可能是 -1？**

编译器可能优化掉未使用的 Uniform；或者拼写/类型/结构体成员名不匹配。应检查是否实际使用、查询时机（link 后）、是否优化；不要把 -1 直接传给 `glUniform*`。

**Q14：为什么 Shader 中要慎用分支？性能一定更差吗？**

GPU 以 SIMD/SIMT 方式并行处理大量线程；同一 warp/wavefront 内线程走不同分支时，硬件可能执行多个分支路径，出现分支发散。片元线程数据不同更容易发散。现代 GPU 有动态分支和 SIMD 调度优化，不能绝对说“有 if 就慢”，但应避免逐像素复杂分支、按像素做不必要的工作；可用 LUT、解析公式、分支合并等优化。

### 3.4 纹理

**Q15：纹理映射的基本流程？**

CPU 加载图像并解码像素 → 创建纹理 → 绑定目标（如 `GL_TEXTURE_2D`）→ `glTexImage2D` 上传 → 设置环绕、过滤、Mipmap → 顶点提供 UV → 顶点着色器传出 UV → 片元着色器 `texture(uTexture, vUV)` 采样。纹理坐标不是像素坐标，通常范围 [0,1]，但环绕模式允许范围外取值。

**Q16：`GL_NEAREST`、`GL_LINEAR`、Mipmap 的区别？**

- NEAREST：选最近纹素，快但锯齿/颗粒明显。
- LINEAR：双线性插值相邻 4 个纹素，放大时平滑，缩小仍可能严重混叠。
- Mipmap：为纹理生成逐级缩小版本，按纹理变化率/距离选择层级，缩小场景显著减少混叠与带宽；minification 通常选 `GL_LINEAR_MIPMAP_LINEAR`。
- 各向异性过滤：改善斜视角远处纹理的模糊，是普通各向同性 mipmap 的扩展。

**Q17：纹理坐标为什么会上下颠倒？**

图像库/UI 坐标系常以左上角为原点、y 向下；OpenGL 纹理坐标传统约定左下角为 (0,0)、y 向上。翻转图片上传数据、翻转 UV，或根据 API/纹理来源正确处理。不要把结论绝对化：不同加载库、FBO 方向、Android SurfaceTexture 都可能影响最终方向。

**Q18：`GL_REPEAT`、`GL_CLAMP_TO_EDGE`、`GL_MIRRORED_REPEAT` 怎么选？**

- REPEAT：周期重复，适合墙纸、地面等平铺图案；接缝可能明显。
- CLAMP_TO_EDGE：边缘纹素延展，适合不希望重复的 UI/单张图。
- MIRRORED_REPEAT：镜像重复，常用于减少周期接缝。
- NPOT（非 2 次幂）纹理在 Core Profile 下可以使用 REPEAT/Mipmap（需满足版本条件，通常 OpenGL 3.0+/ES 3.0 相关规则）；旧版 OpenGL/OpenGL ES 2.0 限制更严格。

**Q19：什么是纹理图集（Texture Atlas）？优缺点？**

把多张小图拼成一张大纹理，不同物体共用同一纹理，减少纹理切换和合批障碍。缺点：图集尺寸/内存、边缘渗色（相邻纹素被过滤混入）、Mipmap 接缝。常用 padding、正确 UV 半像素偏移、限制缩放范围处理。

### 3.5 帧缓冲与后处理

**Q20：默认帧缓冲与 FBO 的区别？FBO 有哪些用途？**

默认帧缓冲由窗口系统管理，最终显示；FBO 是应用可控的离屏帧缓冲，可附加颜色、深度、模板纹理或 renderbuffer。

用途：后处理、渲染到纹理、Shadow Map、G-buffer、环境贴图、多渲染目标（MRT）、GPU 通用计算结果回读等。

**Q21：Renderbuffer 和 Texture 作为附件有什么区别？**

- Renderbuffer：为深度/模板或无需采样的颜色优化，不能直接在 Shader 中采样。
- Texture：可被后续 Pass 采样，适合颜色结果、Shadow Map、G-buffer。
- 深度纹理可被采样；普通深度 renderbuffer 不能当纹理读取。

**Q22：FBO 为什么“黑屏”却无报错？如何排查？**

检查 `glCheckFramebufferStatus(GL_FRAMEBUFFER)` 是否为 `GL_FRAMEBUFFER_COMPLETE`。常见原因：附件尺寸不一致、格式组合不支持、颜色附件未 `glDrawBuffer`、深度附件缺失、纹理未完整初始化、Mipmap level 选错、viewport 为 0、最后忘记 `glBindFramebuffer(GL_FRAMEBUFFER, 0)` 绘制到屏幕。开发期可用 KHR_debug/RenderDoc 捕获附件。

### 3.6 测试、混合与裁剪

**Q23：`glViewport` 和 `glScissor` 的区别？**

- Viewport：把 NDC 映射到窗口/FBO 坐标，相当于改变“画布区域和比例”。
- Scissor：把允许写入的像素限制在矩形内，不改变坐标变换；矩形外丢弃片元。
- 两者可同时使用：先定义映射区域，再限制其中可写像素。

**Q24：深度测试原理？Z-fighting 为什么出现？**

光栅化得到片元深度，与深度缓冲已有值按深度测试函数比较；通过才可能写颜色/深度。OpenGL 默认通常 `GL_LESS`，标准深度范围 [0,1]，但可改 `glClipControl` 和投影约定。

Z-fighting：共面/近共面几何换算到深度量化精度后落到相同或极接近值，排序随视角/插值变化而抖动。解决：避免共面、加微小偏移、用 polygon offset、提高深度精度、控制 near/far 范围、使用 reversed-Z（需正确矩阵和清屏深度）。

**Q25：为什么 near/far 对深度精度影响很大？能推导吗？**

透视投影后做透视除法，深度在非线性空间分布，近处精度高、远处精度低；near 过小会让可用精度迅速恶化。常见近似：均匀间隔深度在 z 处的分辨率为：

```text
Δz ≈ z² / (near * 2^d)   （d 为深度缓冲位数，忽略具体约定）
```

因此不要把 near 设得过小（如 0.0001），也不要无必要把 far 拉得极大；可动态调整或使用 reversed-Z + float depth 改善远平面精度。

**Q26：透明混合怎么实现？为什么要排序和关闭深度写入？**

开启 `GL_BLEND` 并设置源/目标因子。标准非预乘 alpha：

```text
Cout = Csrc * αsrc + Cdst * (1 - αsrc)
αout = αsrc + αdst * (1 - αsrc)
```

半透明物体需要从远到近绘制，否则结果依赖绘制顺序。通常透明 Pass 关闭深度写入、保留深度测试，避免后面的透明片元被已画透明表面挡住。近似排序（按物体距离）不能完美解决交叉/环绕几何，进阶可用 depth peeling、OIT（顺序无关透明）。

预乘 alpha 使用不同混合因子（常见 `GL_ONE, GL_ONE_MINUS_SRC_ALPHA`），颜色和 alpha 的含义不同，不能混用。

**Q27：Alpha Test、Alpha Blend、Alpha-to-Coverage 的区别？**

- Alpha Test：通过 discard/测试硬切透明度，适合树叶、铁丝网；旧版有独立 alpha test，现代常用 Shader `if (alpha < threshold) discard;`。
- Alpha Blend：计算透明混合，不能简单解决所有顺序问题。
- Alpha-to-Coverage：在 MSAA 下把 alpha 转为覆盖率，适合植被边缘抗锯齿，但不是普通透明混合。

**Q28：面剔除（Face Culling）的原理与绕序陷阱？**

根据三角形在屏幕空间的绕序（CW/CCW）判断正面并剔除背面。常见默认正面为 CCW，剔除 CW，但状态可改。若相机矩阵、镜像变换或索引顺序改变，三角形绕序可能反转，导致模型消失。可通过 `glFrontFace` 设置，或对负缩放/镜像物体修正索引顺序。

**Q29：Early-Z 是什么？什么情况会失效？**

GPU 在片元着色器执行前尽可能进行深度测试，提前丢弃被遮挡片元，节省片元着色开销。若片元着色器显式写 `gl_FragDepth`、使用 discard、关闭深度测试等，可能使硬件无法保证 Early-Z（具体取决于 GPU/驱动和程序行为）。所以遮挡关系和无用透明物体仍是性能关键。

**Q30：模板测试能做什么？**

模板缓冲按片元记录掩码，可按比较函数和操作更新。常见用途：轮廓描边、高亮选中区域、体积光/雾的可见区域、镜面反射裁剪、复杂遮罩。流程通常包含 clear stencil、标记 Pass、按模板规则绘制 Pass、恢复状态。

### 3.7 矩阵与坐标空间

**Q31：MVP 的乘法顺序？**

使用列向量和 OpenGL 常见约定：

```text
gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
```

即最右侧先作用：Model → World → View → Clip → NDC。若引擎用行向量或 DirectX 约定，顺序相反，回答前先说明约定。

**Q32：为什么法线不能直接用 Model 矩阵变换？**

法线是方向向量（w=0），且必须保持与切平面垂直。非均匀缩放时 `normal = mat3(model) * n` 会倾斜。正确做法：

```text
normalMatrix = transpose(inverse(mat3(model)))
normalWorld = normalize(normalMatrix * normal)
```

视图空间可传 view*model 的逆转置。若只有均匀缩放或旋转，可用归一化后的 mat3(model)，但仍需注意统一尺度。

**Q33：齐次坐标中点和方向为什么分别用 w=1、w=0？**

点允许平移，w=1；方向不受平移影响，w=0。投影完成后点需要透视除法 `xyz / w`，方向不能简单做同样解释。MVP 中投影矩阵的第四行/第四列就是为透视除法服务。

**Q34：正交投影与透视投影区别？推导时的关键量？**

- 正交：不产生近大远小，裁剪体为长方体；适合 UI、工程/CAD、阴影正交光。
- 透视：符合近大远小，裁剪体为视台/四棱台；关键是 fov、aspect、near、far，以及矩阵如何把 z 映射到裁剪空间并让 w 等于 view-space 深度负值（常见 OpenGL 约定）。

**Q35：相机移动为什么可以等价为对场景做逆变换？**

视图矩阵本质是相机世界变换的逆。相机向右移动，相当于场景在视图空间向左移动。`lookAt(eye, target, up)` 构造相机基向量再形成逆变换。面试常追问为什么不能直接把相机位置当视图矩阵。

### 3.8 渲染架构与高级主题

**Q36：前向渲染（Forward）和延迟渲染（Deferred）的区别？**

| | Forward | Deferred |
|---|---|---|
| 流程 | 几何与光照在同一 Pass，直接算颜色 | 几何 Pass 写 G-buffer，光照 Pass 逐像素采样 G-buffer |
| 多光源 | 需循环/分簇/灯光数量限制 | 更自然支持大量动态光源 |
| 带宽 | 通常较低 | G-buffer 读写带宽高 |
| 透明/MSAA | 相对容易 | 传统 G-buffer 难直接硬件 MSAA，透明需另处理 |
| 材质 | 灵活 | 受 G-buffer 表达和内存限制 |

补充：延迟不等于移动端 TBDR（基于 Tile 的延迟渲染），只是中文名称相近，机制不同。

**Q37：什么是 G-buffer？常见内容？**

几何 Pass 的输出附件集合：位置/深度、法线、albedo、材质参数（金属度、粗糙度）、自发光等。MRT 可一次写多个纹理。设计要权衡精度、带宽、格式与目标硬件。

**Q38：Instancing 的实现与 `gl_InstanceID` 的作用？**

`glDrawElementsInstanced` 一次绘制多个相同网格实例；实例属性放在独立 VBO， divisor 设为 1；Shader 用 `gl_InstanceID` 或 per-instance attribute 获取每个实例的模型矩阵/颜色。适合草、树、建筑、粒子。不是把每个实例 CPU 提交一次。

**Q39：LOD 有哪些实现方式？**

几何 LOD：距离/屏幕尺寸选不同网格。纹理 LOD：Mipmap/虚拟纹理。着色 LOD：远处简化材质/关闭高成本效果。选择依据可结合包围球、屏幕占比、性能预算，并做平滑过渡（morph、dithered fade）避免跳变。

**Q40：Shadow Map 的基本流程和常见问题？**

- 以光源视角渲染深度图。
- 主相机渲染时把世界坐标/视图坐标变换到光源裁剪空间，采样深度并比较。
- 问题：阴影锯齿（PCF/VSM/CSM）、shadow acne（深度偏移/polygon offset）、Peter-panning（偏移过大导致阴影脱节）、远处精度（级联阴影 CSM、稳定级联）、点光源需要 cubemap 或多 pass。

**Q41：MSAA、SSAA、FXAA、TAA 的区别？**

- MSAA：对几何边缘覆盖率多样本抗锯齿，着色通常每像素一次；带宽/存储成本较高。
- SSAA：以更高分辨率渲染再降采样，质量高但成本大。
- FXAA：屏幕空间后处理，成本低但可能模糊细节。
- TAA：利用历史帧和运动向量累积，抗锯齿并稳定细节，但有拖影/重影风险，需要正确处理运动与历史失效。

**Q42：Gamma 校正、sRGB、HDR、Tone Mapping 各解决什么问题？**

- sRGB/Gamma：显示输出和人类感知是非线性；在正确空间做光照和混合。
- HDR：用浮点纹理保留超过 [0,1] 的亮度信息。
- Tone Mapping：把 HDR 动态范围压缩到显示范围，同时保留对比与高光层次。
- 典型流程：纹理解码到线性空间 → 线性光照/HDR 计算 → Tone Mapping → 编码到 sRGB 输出。

**Q43：BRDF 是什么？PBR 的核心？**

BRDF 描述入射辐照度到出射辐射度的方向分布关系。实时 PBR 常用微表面模型：Cook-Torrance 包含分布项 D、几何项 G、菲涅尔项 F，并满足能量守恒；材质参数主要是 albedo、metallic、roughness，配合环境光照（IBL）。回答别只背“更真实”，要说出能量守恒、微表面、线性空间和曝光/色调映射。

### 3.9 API 对比与工程

**Q44：OpenGL 和 Vulkan 的显存管理、同步、多线程有何不同？**

- OpenGL 驱动做较多隐式资源/同步管理，但应用仍必须正确使用 FBO、缓冲区、同步对象等；不是“自动管理显存”。
- Vulkan 要求显式选择内存类型、分配资源、设计 Pipeline/RenderPass、管理 Barrier/Fence/Semaphore。
- OpenGL 上下文限制使多线程录制不如 Vulkan 直接；Vulkan 可多线程并行录制 Command Buffer，但队列提交和同步仍需精心设计。
- 二者都会发生 GPU  stall，如 `glFinish`、无谓回读、同步等待；不要把“OpenGL 都是同步调用”当结论。

**Q45：OpenGL 对象是线程安全的吗？如何多线程使用？**

同一个上下文不能随意多线程并发调用；不同线程可用独立上下文，或用共享上下文共享纹理/缓冲等资源，并按要求同步。资源创建/上传与绘制可拆分，但共享资源的生命周期、Fence/Sync、内存屏障必须正确设计。面试可讲“渲染线程 + 资源加载线程 + 共享上下文 + 同步”方案。

---

## 四、常考数学与光照公式

### 4.1 基础变换

缩放：

```text
S = [sx 0  0  0
     0  sy 0  0
     0  0  sz 0
     0  0  0  1]
```

绕 Z 轴旋转（列向量约定）：

```text
Rz = [ cos -sin 0 0
       sin  cos 0 0
       0    0   1 0
       0    0   0 1]
```

平移：

```text
T = [1 0 0 tx
     0 1 0 ty
     0 0 1 tz
     0 0 0 1]
```

常见组合 `T * R * S` 表示先缩放、再旋转、最后平移；若写反，物体会绕世界原点或出现错误轨迹。

### 4.2 透视投影矩阵（常见 OpenGL 约定）

设 `f = 1 / tan(fovy/2)`，`aspect = width/height`：

```text
[f/aspect  0      0                     0
 0         f      0                     0
 0         0   (far+near)/(near-far)  (2*far*near)/(near-far)
 0         0      -1                    0]
```

这是列主序存储、列向量右乘、裁剪空间 z 映射到 [-w,w] 的常见形式。面试不要求死背时，能说清 fov/aspect/near/far、w=-z_view 和透视除法更重要；若目标 API 使用 [0,1] 深度范围，矩阵不同。

### 4.3 光照模型

Lambert 漫反射：

```text
L = kd * lightColor * max(dot(N, L), 0.0)
```

Phong 高光：

```text
R = reflect(-L, N)
spec = ks * pow(max(dot(R, V), 0.0), shininess)
```

Blinn-Phong：

```text
H = normalize(L + V)
spec = ks * pow(max(dot(N, H), 0.0), shininess)
```

经典环境光 + 漫反射 + 高光：

```text
color = ka * ambient + kd * max(N·L,0) * lightColor
      + ks * pow(max(N·H,0), shininess) * lightColor
```

Blinn-Phong 通常用更宽松的高光指数得到类似效果，计算成本较低；Phong 需要显式计算反射向量。

---

## 五、手写代码题与代码骨架

> 以下代码以 OpenGL 3.3 Core / GLSL 330 core 为例，省略窗口创建、错误检查和资源释放；实际工程必须补齐。代码用于说明流程，不应直接视为完整生产实现。

### 5.1 顶点 + 片元着色器

```glsl
// vertex.glsl
#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aUV;

uniform mat4 uMVP;

out vec2 vUV;

void main()
{
    gl_Position = uMVP * vec4(aPosition, 1.0);
    vUV = aUV;
}
```

```glsl
// fragment.glsl
#version 330 core
in vec2 vUV;

uniform sampler2D uTexture;
out vec4 FragColor;

void main()
{
    FragColor = texture(uTexture, vUV);
}
```

常见追问：

- `aUV` 必须在光栅化前正确插值；若纹理只采样一次但没有对应 UV，会得到固定颜色。
- MVP 只负责顶点位置；法线、UV 需要各自处理。
- `uTexture` 与 CPU 侧纹理单元 0 对应，绘制前要绑定纹理并设置 sampler。

### 5.2 VAO/VBO/EBO 初始化

```cpp
GLuint vao = 0, vbo = 0, ebo = 0;

glGenVertexArrays(1, &vao);
glBindVertexArray(vao);

glGenBuffers(1, &vbo);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

glGenBuffers(1, &ebo);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

// [x, y, z, u, v]
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                      5 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                      5 * sizeof(float), (void*)(3 * sizeof(float)));
glEnableVertexAttribArray(1);

glBindVertexArray(0);

// 每帧
glUseProgram(program);
glBindVertexArray(vao);
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, texture);
glUniform1i(glGetUniformLocation(program, "uTexture"), 0);
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
```

易错点：必须先绑定 VAO 再设置顶点属性；`GL_ELEMENT_ARRAY_BUFFER` 的绑定是 VAO 状态，解绑 VAO 后再解绑 EBO 不会清掉 VAO 中记录的 EBO。

### 5.3 创建并检查 FBO

```cpp
GLuint fbo = 0, colorTex = 0, depthRbo = 0;
const int W = 1024, H = 768;

glGenFramebuffers(1, &fbo);
glBindFramebuffer(GL_FRAMEBUFFER, fbo);

glGenTextures(1, &colorTex);
glBindTexture(GL_TEXTURE_2D, colorTex);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, W, H, 0,
             GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                       GL_TEXTURE_2D, colorTex, 0);

glGenRenderbuffers(1, &depthRbo);
glBindRenderbuffer(GL_RENDERBUFFER, depthRbo);
glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, W, H);
glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                          GL_RENDERBUFFER, depthRbo);

glDrawBuffer(GL_COLOR_ATTACHMENT0);

if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    // 记录状态码并处理错误
}

glViewport(0, 0, W, H);
// 离屏绘制完成后：
glBindFramebuffer(GL_FRAMEBUFFER, 0);
// 若之后绘制默认帧缓冲，恢复对应 viewport
```

追问：若后续要采样阴影，改用深度纹理；MRT 要配置多个附件并对应 Shader 的多个输出 location。

### 5.4 手写/补全 MVP 更新

```cpp
// 伪代码：列主序 + 列向量
uMVP = projection * view * model;
glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(uMVP));

// 法线（假设传世界空间法线）
uNormalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
glUniformMatrix3fv(normalLocation, 1, GL_FALSE,
                   glm::value_ptr(uNormalMatrix));
```

`GL_FALSE` 表示不要求 OpenGL 对数据做转置；若 CPU 数组已是 OpenGL 可用的列主序，不应随便转置。手写矩阵题尤其要说明存储约定。

### 5.5 经典矩阵输出题

```cpp
glm::mat4 M = glm::translate(glm::mat4(1.0f), {5, 0, 0})
            * glm::scale(glm::mat4(1.0f), {2, 2, 2});
```

点 `(1,0,0,1)` 变为 `(7,0,0,1)`：先缩放到 2，再平移 5。若矩阵写成 `S*T`，则先平移后缩放，结果变成 12。这几乎是笔试固定陷阱。

### 5.6 可选的进阶手写题

- 实现简版 `Mat4`：乘法、平移、旋转、缩放、透视、lookAt。
- 实现三角形光栅化：包围盒、质心坐标、深度插值、纹理采样。
- 实现 Z-Buffer：逐像素比较并更新深度。
- 实现一个全屏后处理 Pass：FBO 场景纹理 → 全屏三角形 → 灰度/模糊/边缘检测。
- 实现 Instancing：树/草实例矩阵数组 + `gl_InstanceID`。

---

## 六、OpenGL ES / 移动端专题

**Q1：OpenGL ES 和桌面 OpenGL 的主要区别？**

面向嵌入式/移动的精简 API，Core/Profile 版本与 GLSL ES 版本对应关系不同；ES 2.0 起主要使用可编程管线；GLSL ES 对精度、纹理格式、framebuffer 附件组合有更严格限制。不要直接把桌面 GLSL 的 `#version 330 core` 搬到 ES 环境。

**Q2：EGL、GLSurfaceView、GLThread 各负责什么？**

- EGL：连接原生窗口 Surface 与 OpenGL ES 上下文，负责显示缓冲/交换等。
- GLSurfaceView：Android 封装渲染 Surface、EGL 生命周期和渲染线程的控件。
- GLThread：内部持续渲染的线程；GLSurfaceView 的渲染回调在该线程执行，不是 UI 线程。

**Q3：为什么 OpenGL ES 调用必须在正确的线程？**

上下文是线程相关的。一个线程要使用当前上下文；在不同线程间移动上下文需要 makeCurrent/release 并同步。Android 上 UI 更新仍要回主线程，不能因为“有 GLThread”就在渲染线程直接改 View。

**Q4：GLSL ES 的 `precision` 是什么？为什么片元着色器常写 `mediump float`？**

声明 float 精度范围/实现质量。移动端片元着色器没有桌面那样的默认 float 精度，需显式指定，否则某些版本编译失败。`highp` 精度高但可能更慢/不支持（取决于硬件和 ES 版本）；颜色可用 mediump，位置/矩阵运算要按数值范围谨慎选择。

**Q5：Android/GLES 黑屏的排查顺序？**

1. EGL 配置/上下文/Surface 是否有效，是否 makeCurrent。
2. Shader 编译/链接是否成功，日志是否完整。
3. 顶点数据、VAO/VBO 绑定和属性 location 是否正确。
4. MVP、UV、纹理单元是否正确。
5. viewport、面剔除、深度/混合、FBO 状态是否正确。
6. 纹理是否上传、是否翻转、格式是否匹配。
7. 是否忘记交换缓冲/请求渲染，或渲染线程已退出。

**Q6：移动端为什么常用 TBDR？与 Deferred Shading 有什么区别？**

TBDR（Tile-Based Deferred Rendering）是 GPU 硬件架构：把屏幕分成 Tile，尽量在片上/高速缓存中完成几何、片元与深度处理，减少对主存的读写。Deferred Shading 是应用/算法层面的 G-buffer + 光照 Pass。两者名字相近但层级不同，不能混答。

**Q7：移动端优化有哪些？**

纹理压缩（ASTC/ETC2/PVRTC，按平台选）、合理 Mipmap、减少纹理尺寸和带宽、网格 LOD、Instancing/合批、减少透明与 Overdraw、避免频繁 FBO 切换、控制 CPU 每帧分配、减少每帧 Uniform/纹理上传、用高质量渲染测量而不是凭感觉。

**Q8：相机纹理为什么常有外部纹理/OES？**

Android Camera/SurfaceTexture 的 GPU 数据通路可能以外部纹理形式提供，采样器要用 `samplerExternalOES` 和扩展，不能当普通 `sampler2D` 直接采样；还要正确处理 SurfaceTexture 变换矩阵、方向和时序。

---

## 七、性能优化与故障排查

### 7.1 优化要从测量开始

先确认瓶颈：CPU 提交、顶点处理、片元处理/Overdraw、带宽、同步 Stall。工具：RenderDoc（帧捕获与管线状态）、NVIDIA Nsight、AMD Radeon GPU Profiler、Xcode Metal/OpenGL 工具、Android GPU Inspector、基础计时查询。不要一卡就回答“减少 Draw Call”。

### 7.2 常见现象 → 原因 → 处理

| 现象 | 常见原因 | 检查/处理 |
|---|---|---|
| 全黑 | Shader 失败、VAO 错误、MVP 错误、纹理未绑定、FBO 不完整 | 看编译/链接日志，可视化中间附件，检查矩阵与 viewport |
| 物体跑到屏幕角落 | MVP 顺序错、列/行主序错、透视矩阵 near/far 写错 | 打印变换后的裁剪坐标，核对约定 |
| 纹理上下颠倒 | 图像原点与纹理坐标约定不一致 | 翻转图片数据或 UV，确认 SurfaceTexture 变换 |
| 远处闪烁/摩尔纹 | 缺少 Mipmap、过滤不当、纹理采样高频 | min filter 使用 mipmap，各向异性过滤，TAA |
| 表面闪纹 | Z-fighting | 避免共面、偏移、深度精度、reversed-Z |
| 透明物体穿插错误 | 未排序/深度写入错误 | 不透明→透明，远→近，透明关深度写 |
| 模型消失 | 面剔除绕序反了、镜像负缩放、背面法线错误 | 临时关闭 culling，检查索引/绕序 |
| FBO 内容全白/未绘制 | 附件未完整、DrawBuffer 未设置、viewport 为 0 | 检查 FBO status 与附件配置 |
| 性能突然下降 | 分支发散、过多采样、纹理带宽、Shader 重编译 | Profile，检查逐像素工作量与状态切换 |

### 7.3 面试回答优化的结构

1. **现象与指标**：GPU 时间、Draw Call、三角形数、纹理带宽、CPU 帧时间。
2. **定位工具**：RenderDoc/Nsight/查询对象/性能计数器。
3. **根因**：哪个阶段、哪种资源、哪种同步成为瓶颈。
4. **方案与权衡**：Instancing/合批、LOD、纹理压缩、减少 Overdraw、FBO/MRT 带宽优化等。
5. **验证**：优化前后同场景对比，确保画质和正确性没有回归。

---

## 八、复习路线与项目准备

### 8.1 建议顺序（对应 AuroraShader 题库路线）

1. 渲染管线与 GPU 工作方式。
2. 坐标空间、MVP、矩阵约定。
3. 光栅化、插值、深度缓冲与精度。
4. 法线、切线空间、法线贴图。
5. 纹理、过滤、Mipmap、图集。
6. 颜色空间、Gamma、HDR、Tone Mapping。
7. 前向/延迟与 G-buffer。
8. 光照、BRDF、PBR。
9. Shadow Map、PCF、CSM、bias/acne。
10. GPU Profiling 与图形调试。
11. 刷 115 道面试题库并口述答案。

### 8.2 项目怎么准备

至少准备一个能演示、能画架构图、能讲性能数据的项目，例如：

- 基于 LearnOpenGL 的模型查看器：VAO/VBO、Assimp、材质、多光源。
- 离屏渲染 + 后处理：FBO、Bloom、Gamma、Tone Mapping。
- Shadow Map 或 CSM 场景：能讲 depth texture、bias、PCF、性能取舍。
- 延迟渲染 Demo：G-buffer、多光源、为什么透明单独处理。
- OpenGL ES 相机预览/滤镜：EGL、纹理方向、OES 外部纹理、渲染线程。
- tinyrenderer 改造：手写 MVP、三角形光栅化、Z-Buffer、纹理采样。

每个项目都准备：**背景 → 架构 → 一个技术难点 → 如何定位 → 解决方案 → 性能/画质结果 → 反思**。比“看过教程”更有说服力的是能解释某一帧到底如何产生。

### 8.3 面试答题模板

- 概念题：一句话定义 → 管线位置 → 为什么需要 → 一个实际例子 → 一个坑。
- 计算题：先声明坐标/矩阵约定，再写公式，最后代入数值验证。
- 优化题：先测量，再定位，再权衡，最后给对比数据。
- 不会的问题：明确边界，讲相关已知部分和排查思路，不要硬编 API 行为。

---

## 九、参考资料

### 近期中文资料

- [图形学面试题总结（2025-03）](https://blog.csdn.net/qq_46348003/article/details/146211115)
- [游戏研发笔试备考：从 C++ 到图形学的考点拆解（2026-10）](https://blog.csdn.net/weixin_29007243/article/details/167731114)
- [搜狐畅游技术美术校招笔试复盘（2026-08 更新）](https://blog.csdn.net/weixin_34268169/article/details/92110339)
- [OpenGL 面试核心：坐标变换、渲染管线与动画数学（2026-11）](https://blog.csdn.net/weixin_36300623/article/details/170404840)
- [OpenGL 面试：试描述现代 OpenGL 渲染管线（经典）](https://m.blog.csdn.net/qq_43535469/article/details/118895753)
- [几道常见的 OpenGL 面试题（2024-05）](https://cloud.tencent.com/developer/article/2418145)
- [Android 自定义 View 中 OpenGL ES 实战（2026-10）](https://bbs.csdn.net/weixin_29193259/article/details/100712994)
- [OpenGL ES 嵌入式图形渲染核心原理与实践（2025-11）](https://blog.csdn.net/qq_40882017/article/details/155279450)
- [OpenGL/ES 上下文与线程等文章分类](https://m.blog.csdn.net/Kennethdroid/category_9120765.html)
- [计算机图形学课程试卷考点整理（2025-03）](https://wenku.csdn.net/doc/2qotsxvqh5)

### GitHub 题库与学习路线

- [AuroraShader：图形学面试学习路线](https://github.com/GloryKazimierz/AuroraShader)
- [115 道图形学面试题库（中文）](https://github.com/GloryKazimierz/AuroraShader/blob/main/docs/interview/12_GRAPHICS_INTERVIEW_QUESTION_BANK.zh-CN.md)
- [AuroraShader：面试课程中文目录](https://github.com/GloryKazimierz/AuroraShader/blob/main/docs/interview/README.zh-CN.md)
- [prographon/graphics-developer-roadmap](https://github.com/prographon/graphics-developer-roadmap)
- [渲染引擎/图形学学习路径与仓库推荐（2026-07）](https://blog.csdn.net/2301_79730987/article/details/163136163)
- [tinyrenderer：手写软光栅器](https://github.com/ssloy/tinyrenderer)

### 系统教程与权威文档

- [LearnOpenGL CN](https://learnopengl-cn.github.io/)
- [LearnOpenGL 配套源码](https://github.com/JoeyDeVries/LearnOpenGL)
- [Khronos：Rendering Pipeline Overview](https://www.khronos.org/opengl/wiki/Rendering_Pipeline_Overview)
- [Khronos OpenGL Wiki](https://www.khronos.org/opengl/wiki/)
- [docs.gl：OpenGL API 文档](https://docs.gl/)
- [OpenGL 官方 SDK 与资源](https://www.opengl.org/sdk/)
- [Real-Time Rendering](https://www.realtimerendering.com/)

> 版权提示：以上链接仅供学习导航。引用他人文章或代码时请遵守原作者许可；本 Markdown 中的答案和代码为归纳与示例整理。
