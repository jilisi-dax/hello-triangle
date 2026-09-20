# 引擎化路线图：从 demo 到 mini 游戏引擎

> 目标：把 `projects/01-hello-triangle` 逐步演进成一个功能完整、设计接近商用引擎的
> mini 游戏引擎（对标个人规模的 "mini Unity"），最后用它做出一个完整的小游戏。
>
> 时间估算按 **每天 1~2 小时（每周约 10 小时）** 的节奏：
> - **MVP 引擎**（阶段 0~6 精简版）：约 3~4 个月
> - **完整版**（含编辑器 + 做出一个游戏）：约 8~12 个月
> - 全职学习（每天 4~6 小时）大约除以 3。
>
> 每个阶段都有「验收标准」，做完就打勾，不要跳。

---

## 总览

| 阶段 | 名称 | 核心内容 | 预估（每周10h） |
|---|---|---|---|
| 0 | 工程与地基 | git、目录重构、还清现有架构债 | 1~2 周 |
| 1 | 渲染能力补完 | 鼠标相机、完整光照、模型类 | 3~4 周 |
| 2 | 引擎架构成型 | ECS、游戏循环、输入/事件、资源管理 | 4~6 周 |
| 3 | 高级渲染 I | 帧缓冲、天空盒、阴影、法线贴图 | 4~5 周 |
| 4 | 高级渲染 II | HDR/泛光、instancing、文字、粒子 | 3~4 周 |
| 5 | 物理与碰撞 | 自写 AABB → 接入 Bullet/Jolt | 2~4 周 |
| 6 | 运行时系统 | 音频、序列化、脚本组件 | 3~5 周 |
| 7 | 编辑器 | ImGui、Inspector、Gizmo、Play 模式 | 4~6 周 |
| 8 | 做一个游戏 | 用自己的引擎做完整的小游戏 | 3~5 周 |
| 9 | 打磨与进阶 | 性能优化、(可选) Android GLES | 长期 |

---

## 阶段 0：工程与地基（1~2 周）

**目标**：还清现在的债，让后面每一层楼都盖在实地上。

- [ ] `git init`，写 `.gitignore`（忽略 `.vs/`、`build/`、`x64/`），从此每个功能一个 commit
- [ ] 删除死文件：空的 `main.cpp`、`randerComponent.cpp`、`main - 副本.cpp`、`main_bak.cpp`（想留教材就挪到 `docs/`）
- [ ] 目录重构成「引擎 / 游戏分离」的雏形：
  ```
  engine/    ← 以后所有引擎代码都进这里
    core/      通用基础（log、assert、数学工具）
    renderer/  shader、texture、mesh、camera
    scene/     Scene、SceneObject、Component
  sandbox/   ← 你的练习主程序（现在的 main1.cpp）
  assets/    ← 模型、贴图（现在的 asset/）
  shaders/
  ```
- [ ] 还清架构债：
  - [ ] `SceneObject` 的 position/rotation/scale 真正生成 model 矩阵传给着色器（`randerComponent.h` 里硬编码的矩阵删掉）
  - [ ] 同步传 normalMatrix（model 的逆转置），修 `role.vert` 里 `Normal = aNormal` 的隐患
  - [ ] `LightComponent` 接线：`MeshRendererComponent::draw` 里写死的 `(-2,-2,-5)` 改成从组件读
  - [ ] 主循环调用 `scene.update(dt)`，让 Component::update 真正跑起来
  - [ ] `CubeRenderer` 析构补删 EBO；`sys.h` 的函数加 `inline`；着色器路径统一走 `getAssetPath()`
  - [ ] 相机 aspect 从 framebuffer 实际宽高取，窗口缩放不再拉伸
- [ ] 写一个极简日志宏（LOG_INFO/LOG_ERROR，带时间戳），替换散落的 printf

**验收**：场景里三个物体全部通过 transform 数据摆放；改 LightComponent 的位置，画面光照跟着变；git 历史干净。

---

## 阶段 1：渲染能力补完（3~4 周）

**目标**：补完 learnopengl 光照与模型章节，让引擎"能画复杂东西"。

- [ ] 鼠标视角（yaw + pitch，GLFW cursor callback）+ 滚轮缩放 fov
- [ ] 完整 Blinn-Phong：镜面反射（高光）、`Material` 类（ambient/diffuse/specular/shininess）
- [ ] 三种光源组件化：DirectionalLight、PointLight（带衰减）、SpotLight（聚光）
- [ ] `Mesh` / `Model` 类；模型加载换 assimp（或把手写 OBJ loader 升级成支持索引 + 多网格）
- [ ] 理解并开启：面剔除、深度测试的状态管理
- [ ] 学会用 **RenderDoc** 抓帧看 drawcall——以后调图形 bug 的第一工具

**资料**：learnopengl《光照》《模型加载》《深度测试》章节；TheCherno 的 OpenGL 系列

**验收**：一个场景，树 + 地面 + 多个立方体，平行光 + 两个彩色点光 + 手电筒聚光，材质各不相同。

---

## 阶段 2：引擎架构成型（4~6 周）★ 全路线最重要的一步

**目标**：从"面向对象拼积木"升级成"数据驱动的引擎结构"。做完这步，你的项目才配叫引擎。

- [ ] **ECS 落地**：拆掉 `modelObj/cubeModel/ground` 这类继承子类，改为「实体 = ID，组件 = 数据，系统 = 逻辑」。建议先读懂 [EnTT](https://github.com/skypjack/entt) 的用法再决定：直接用，或照着原理手写简化版（学得更深）
- [ ] **游戏循环**：可变步长渲染 + 固定步长逻辑（accumulator 模式，物理预备）
- [ ] **输入系统**：GLFW 回调 → `Input::IsKeyDown(Key::W)` 这样的平台无关接口
- [ ] **事件系统**：简单事件总线（WindowResize/KeyPressed/…），发布-订阅
- [ ] **窗口/平台层抽象**：`Window` 基类 + `WindowsWindow` 实现，引擎代码不再直接碰 GLFW
- [ ] **资源管理**：Texture/Mesh/Shader 用「句柄 + 管理器 + 缓存」，同名资源只加载一次，删除走引用计数
- [ ] **渲染器接口化**：`RendererAPI` 抽象（Init/Clear/DrawIndexed…），OpenGL 只是其中一个后端——这是商用引擎 RHI 思想的雏形
- [ ] （此时把 vcxproj 迁到 **CMake**，engine 编成静态库 + sandbox 链接——以后转 Android 必须走这步）
- [ ] `main.cpp` 瘦身到 100 行以内：场景全部用「实体 + 组件 + 参数」搭出来

**资料**：TheCherno 的 **Hazel 引擎系列**（照着做一遍引擎，前 60 集左右）；GAMES104 第 2~3 课

**验收**：新增一种物体（比如旋转的风车）时，只需要写一个组件类 + 在场景配置里加一行，不用碰任何引擎代码。

---

## 阶段 3：高级渲染 I（4~5 周）

**目标**：learnopengl 高级 OpenGL 前半部，画面上台阶。

- [ ] 帧缓冲（FBO）离屏渲染 → 后处理入口（灰度/反色/核卷积锐化练手）
- [ ] 天空盒（cubemap）
- [ ] 阴影贴图（平行光 shadow mapping；可选：点光六面阴影）
- [ ] 法线贴图、视差贴图
- [ ] 相机做成组件 + 支持多相机（正交/透视切换，为编辑器视图做准备）

**验收**：场景有天空盒、地面投影、墙面法线贴图凹凸感，按 F1 开关一种后处理。

---

## 阶段 4：高级渲染 II 与特效（3~4 周）

- [ ] HDR + 泛光（bloom）+ gamma 校正（完整后处理管线）
- [ ] Instancing：一帧画 10 万个实例（理解 drawcall 开销）
- [ ] 文字渲染（FreeType）：FPS 计数、调试文本
- [ ] CPU 粒子系统组件化（发射器/生命周期/重力，渲染用 instancing）
- [ ] （可选 +3 周）骨骼动画：换 glTF + assimp 骨骼导入，播放走路/跑步动画

**验收**：发光物体带 bloom 泛光、10 万草叶随风摆、屏幕角上有 FPS、篝火粒子。

---

## 阶段 5：物理与碰撞（2~4 周）

- [ ] 自写基础版（必须先做，理解原理）：AABB / 球体碰撞检测、位置修正 + 简单冲量响应、**射线检测**（编辑器点选用）
- [ ] 接入成熟库（Bullet 或 Jolt）：RigidBody、BoxCollider/SphereCollider 组件、物理固定步长接入游戏循环

**资料**：Randy Gaul《How to Create a Custom Physics Engine》系列短文

**验收**：一个球从空中落到棋盘地面，弹两下滚走；用鼠标点击屏幕能拾取到物体。

---

## 阶段 6：运行时系统（3~5 周）

- [ ] 音频：[miniaudio](https://github.com/mackron/miniaudio)（单头文件），AudioSource/AudioListener 组件，3D 声音衰减
- [ ] 场景序列化：实体 + 全部组件保存为 JSON（nlohmann/json 或 yaml-cpp），启动加载还原
- [ ] 脚本组件：先做 native C++（`onCreate/onUpdate/onDestroy` 虚函数），进阶再上 Lua（sol2）热加载
- [ ] 对象生命周期规范化：延迟销毁队列（不能在遍历 ECS 时直接 delete）

**验收**：编辑场景 → 存成 json → 删代码里搭场景的部分 → 从 json 完整还原；碰到金币播放音效。

**★ MVP 达成线**：到这里你已经有「数据驱动 + 物理 + 存档 + 音频」的可用引擎，可以跳过 7、8 直接进 9，或者继续冲完整版。

---

## 阶段 7：编辑器（4~6 周）

**目标**：商用引擎的灵魂——不写代码就能搭场景。

- [ ] [Dear ImGui](https://github.com/ocornut/imgui) 停靠布局接入（作为引擎的一个 Layer，编辑器与游戏同进程不同层）
- [ ] Hierarchy 面板（实体树）+ Inspector 面板（查看/编辑选中实体的所有组件属性）
- [ ] ImGuizmo：视口里平移/旋转/缩放手柄，配合射线拾取点选物体
- [ ] 资产浏览器（浏览 assets/，拖拽贴图到材质）
- [ ] Play/Stop 模式：进入播放前序列化场景快照，停止时还原（商用引擎同款思路）
- [ ] 网格线、相机预览小窗等视口辅助

**资料**：Hazel 系列编辑器部分；ImGui 自带 demo 就是说明书

**验收**：纯鼠标操作，从空场景拼出「地面 + 树 + 光源 + 玩家」并按 Play 运行。

---

## 阶段 8：用它做一个完整小游戏（3~5 周）

**目标**：引擎的毕业考试。选题建议 3D 滚球收集金币（Unity 经典教程同款，覆盖面广）。

- [ ] 玩家控制（物理 + 输入 + 相机跟随）、金币收集判定、计分 HUD（文字渲染）
- [ ] 主菜单 / 暂停 / 胜利失败界面、音效 BGM、重新开始
- [ ] 关卡数据全部放 json（验证序列化管线）
- [ ] Release 打包：exe + 资源目录，发给别人能双击就玩

**验收**：一个不懂编程的朋友拿到压缩包，能玩通并知道怎么赢。

---

## 阶段 9：打磨与进阶（长期，按需选做）

- [ ] 性能：批量渲染合批、视锥剔除、接入 Tracy/Optick profiler、GPU 计时查询
- [ ] 多线程：后台线程加载资源
- [ ] RHI 抽象补完 → 学习 Vulkan（有了自己的 RendererAPI 接口，迁移才有可能）
- [ ] **Android GLES 3.3 移植**（你的原始目标）：Platform 层换 Android + EGL，CMake 在阶段 2 已备好
- [ ] 理论补强：GAMES104 全课、书《Game Engine Architecture》、读 Piccolo（腾讯开源引擎）源码

---

## 贯穿始终的习惯

1. **每个功能一个 git commit**，阶段结束打 tag——引擎是长跑，历史就是你的保险。
2. **图形 bug 先开 RenderDoc** 再猜：看 drawcall、看绑定状态，一抓一个准。
3. **每阶段收尾跑一个 demo**，截图 + 三行说明存进 `CHANGELOG.md`，激励自己。
4. 遇到设计拿不准：**看 Hazel 怎么做**（结构和你的路线几乎一一对应），但理解后用自己的方式写。
5. 现在这个 demo 永远保留一份 tag（`v0.1-hello-engine`），以后回头看会很有成就感。
