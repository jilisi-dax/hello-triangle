# 从 OpenGL 学习到 mini 游戏引擎

这是一个"边学边造"的项目：从画第一个三角形开始，按商用引擎（Unity/UE/Godot）的
架构思想——组件化、数据驱动材质、资源管理、多 pass 管线——一步步把手写 demo
长成一个个人规模的 mini 游戏引擎，最后用它做出一个完整的小游戏。

目标不是抄教程，而是通过亲手实现每一层，搞懂**游戏、引擎、渲染是怎么跑起来的**。

- 路线图（怎么走、走到哪）：[ROADMAP.md](ROADMAP.md)
- 学习日志（按日期的轨迹）：[learnLog.md](learnLog.md)
- 带教约定（教学方式）：[AGENTS.md](AGENTS.md)

## 当前进度

**阶段 0~2 已完成，正在阶段 3「高级渲染 II」（HDR / PBR / 泛光）。**

渲染能力：

- 完整 Blinn-Phong 光照：点光 / 聚光 / 距离衰减 / 软边
- 阴影贴图（shadow mapping）：PCF 软边、slope bias / 背面剔除 / normal offset 抗痤疮
- HDR 天空盒：全景图（.hdr）→ cubemap 离线转换，方向采样、去平移、钉深度
- 法线贴图：切线空间、TBN 矩阵、Lengyel 公式求切线
- 帧缓冲后处理：离屏 RT → 全屏 quad → 灰度 / 反色 / 3×3 卷积锐化（F1~F3 切换）
- lit uber shader：全部材质共用一个 shader，参数由材质 JSON 开关

引擎架构：

- Scene / SceneObject / Component 组件化结构，Transform 驱动 model 矩阵
- Material 材质 JSON 数据驱动；ResourceLib 资源缓存（同名只加载一次）
- Input（平台回调层）→ ActionMap（动作映射层）两级输入，电平 / 边沿两套查询
- 手写 OBJ 加载器（扇形三角化、非法格式拒绝）；程序化 mesh（cube / sphere / ground）

## 快速开始

环境：Windows + Visual Studio（MSVC）。GLFW 3.4 与 GLAD 已内置于 `thirdparty/`，
不依赖任何系统级安装。

1. 进入 `projects/01-hello-triangle/`
2. 双击 **hello-triangle.sln** 打开工程
3. **F5** 调试运行（或 Ctrl+F5 直接运行）

操作：**WASD** 移动、**鼠标**转视角、**滚轮**缩放 fov、**F1/F2/F3** 切换
灰度/反色/锐化后处理（再按一次还原）、**Esc** 退出。

命令行构建（不开 IDE）：`build-cmdline.cmd Debug` 或 `build-cmdline.cmd Release`。

## 当前场景

棋盘格地面 + 金属立方体 + OBJ 角色模型 + 三颗球（法线贴图砖纹 / 金属自转 / 塑料）
+ HDR 天空云天 + 三盏灯（含正上方聚光投影）。全部由 `main.cpp` 的 `init()` 用
「场景对象 + 组件 + 材质 JSON」拼装。

## 目录结构

```
学习openGL/
├─ ROADMAP.md / learnLog.md / AGENTS.md   路线图 / 学习日志 / 带教约定
├─ projects/01-hello-triangle/            主工程
│  ├─ Client/                             全部 C++ 源码（引擎雏形 + 场景）
│  ├─ asset/                              着色器、材质 JSON、贴图、模型、天空盒
│  ├─ hello-triangle.sln / .vcxproj       VS 工程文件
│  └─ build-cmdline.cmd                   命令行构建脚本
├─ thirdparty/                            GLFW 3.4（bin）、GLAD（源码）
└─ tools/glad-generator/                  glad 生成器（将来出 GLES 版加载器用）
```

## 工程配置要点

- `hello-triangle.vcxproj`：`/utf-8`（源码含中文注释）、`/MD`（与 GLFW 预编译库
  CRT 一致）、头/库路径指向 thirdparty、输出统一在 `build\`
- 新建 `.cpp` 必须登记进 vcxproj 才会编译；新文件保存为 **UTF-8 带签名**
- 着色器（`asset/shaders/`）是**运行时读取**的：改 GLSL 存盘后直接切回窗口即可
  看效果，不需要重新编译 C++
- glfw3.dll 每次编译自动复制到输出目录

## 学习资料

| 资料 | 用途 |
|---|---|
| [learnopengl 中文版](https://learnopengl-cn.github.io/) | 图形渲染主线教程，路线图逐章映射 |
| [GAMES104《现代游戏引擎》](https://www.bilibili.com/)（B 站，王希） | 引擎架构理论：为什么这么设计 |
| [Hazel 引擎（TheCherno）](https://github.com/TheCherno/Hazel) | 引擎实作参照，结构与路线几乎一一对应 |
| [Piccolo（腾讯开源）](https://github.com/BoomingTech/Piccolo) | GAMES104 配套引擎，读源码用 |
| [Poly Haven](https://polyhaven.com/) | 免费 HDR 全景图 / PBR 材质素材 |

## 常见问题

- **F5 报"找不到 glfw3.dll"** → 先完整 Build 一次，编译后事件会自动复制 DLL。
- **改了 shader 没变化** → 确认存盘；shader 按路径缓存，重启程序即重新加载。
- **新加的 cpp 报链接错误（找不到函数）** → 忘了登记进 vcxproj。
- **画面异常先抓帧** → RenderDoc 打开 exe 抓一帧，看 drawcall 和绑定状态。
