# OpenGL 学习环境（Visual Studio 2026 / C++ / GLFW / GLAD）

基于你机器上的 **Visual Studio Community 2026** 搭建的 OpenGL 开发环境。
第三方库（GLFW、GLAD）都在本目录内，不依赖任何系统级安装；编译器用的是
VS 自带的 MSVC 工具集。

## 快速开始

1. 打开资源管理器，进入 `学习openGL\projects\01-hello-triangle\`
2. 双击 **hello-triangle.sln** → Visual Studio 打开工程
3. 按 **F5**（调试运行）或 **Ctrl+F5**（不调试直接运行）

弹出窗口：一个左右摆动的彩色三角形，控制台同时打印你的 OpenGL 版本和
显卡型号（RTX 3080 Ti / OpenGL 3.3）。按 **ESC** 退出。

- 改完 `main.cpp` 直接再按 F5，VS 会自动重新编译。
- 想打断点：点代码行左侧灰色边栏出现红点，按 F5，程序会在断点处停住，
  可以看变量、单步执行——这是学 OpenGL 时最有用的调试手段。
- glfw3.dll 会在每次编译后自动复制到输出目录，无需手动管理。

## 目录结构与大小

| 位置 | 大小 | 内容 |
|---|---|---|
| `projects/01-hello-triangle/` | <1M | demo 工程：main.cpp（逐行中文注释）+ .sln/.vcxproj 工程文件 + build-cmdline.cmd |
| `thirdparty/glfw-3.4.bin.WIN64/` | ~10M | GLFW 窗口库（用到的是 include/ 和 lib-vc2022/） |
| `thirdparty/glad/` | 1.5M | GLAD 加载器源码（glad.h + glad.c），你的程序 #include 它 |
| `tools/glad-generator/` | 3.8M | Python 版 glad 生成器，以后要生成 OpenGL ES 版加载器时用 |

## 工程文件说明

- `hello-triangle.sln` —— 解决方案，双击它打开整个工程
- `hello-triangle.vcxproj` —— 工程配置（Debug/Release × x64），已经配好：
  - 头文件路径（GLAD、GLFW）
  - 库路径（GLFW 的 lib-vc2022）和链接库（glfw3.lib、opengl32.lib 等）
  - `/utf-8` 编译选项（main.cpp 是 UTF-8 编码，含中文注释）
  - `/MD` 运行时库（与 GLFW 预编译库的 CRT 链接方式保持一致）
  - 输出统一放 `build\` 目录，调试工作目录也指向输出目录
- `build-cmdline.cmd` —— 不开 IDE 的命令行构建方式：
  `build-cmdline.cmd Debug` 或 `build-cmdline.cmd Release`

## demo 代码怎么读

`main.cpp` 按 5 个步骤组织，每步都有分隔注释：

1. 初始化 GLFW，创建窗口（3.3 核心模式）
2. 用 GLAD 加载所有 OpenGL 函数指针
3. 编写着色器 + 上传顶点数据（VBO/VAO —— 现代 OpenGL 的核心概念）
4. 渲染循环（清屏 → 设 uniform → 画三角形 → 交换缓冲）
5. 清理资源

## 如何新建第二个练习工程

最简单的方式：把 `01-hello-triangle` 文件夹整个复制一份、改名（如
`02-shaders`），双击里面的 .sln 打开，把 main.cpp 换成教程新章节的代码
即可（.sln/.vcxproj 里的路径都是相对路径，复制后依然有效；工程显示名
仍叫 hello-triangle，不影响使用，想改可在 VS 里"重命名"）。

## 学习路线建议

本环境对应教程 **learnopengl**（官方中文版）：https://learnopengl-cn.github.io/

建议顺序：入门 → 着色器 → 纹理 → 变换 → 坐标系统 → 摄像机 → 光照 →
模型加载 → 高级 OpenGL。每章代码都可以用"复制工程文件夹"的方式直接实践。

之后若转向 Android（OpenGL ES），`tools/glad-generator` 可重新生成 GLES
版加载器，C++ 着色器知识全部通用。

## 常见问题

- **按 F5 报"找不到 glfw3.dll"** → 先完整编译一次（Build → Build Solution），
  编译后事件会自动复制 DLL。
- **加载纹理时文件找不到**（以后会学到）→ 工程的调试工作目录已指向
  `build\Debug\`，把图片放在 exe 旁边即可。
- **VS 正在更新时编译失败** → 等 Visual Studio Installer 更新完成再编译。
