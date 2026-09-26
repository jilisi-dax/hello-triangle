# 路线图：从 OpenGL 学习到 mini 游戏引擎

> **目标**：通过一步步自己学习 OpenGL，把一个"画三角形"的 demo 逐层长成
> 架构与商业游戏引擎同构的完整引擎（个人规模的 mini Unity），最后用它做出
> 一个完整的小游戏——在这个过程中彻底搞懂**游戏、引擎、渲染是怎么跑起来的**。
>
> **双主线学习资料**
> - 图形与渲染（动手实现每一课）：learnopengl 官方中文 https://learnopengl-cn.github.io/
> - 引擎与架构（理解商用引擎为什么这么设计）：GAMES104《现代游戏引擎》（B 站，王希），
>   配套开源引擎 Piccolo（https://github.com/BoomingTech/Piccolo）
> - 架构实作参照：TheCherno 的 Hazel 引擎系列（https://github.com/TheCherno/Hazel）
>
> **节奏口径**：每天 1~2 小时（每周约 10 小时）。MVP 引擎（阶段 0~7 精简）约 5~6 个月，
> 完整版（含编辑器 + 毕业游戏）约 10~14 个月，全职学习大约除以 3。
>
> **执行原则**：一步一验收，跑通再走；重构必须"行为与改前完全一致"；
> 每完成一课在 learnLog.md 记一行；每完成一阶段回到本文档打勾，不跳步。

---

## 总览

| 阶段 | 名称 | 状态 | 核心内容 | 预估（每周10h） |
|---|---|---|---|---|
| 0 | OpenGL 入门 | ✅ | 窗口、几何、shader、纹理、变换、相机 | — |
| 1 | 光照与引擎雏形 | ✅ | Blinn-Phong、组件化、材质 JSON、资源库、输入 | — |
| 2 | 高级渲染 I | ✅ | FBO 后处理、天空盒、阴影、法线贴图、uber shader | — |
| 3 | 高级渲染 II | **← 当前** | HDR/泛光、PBR、IBL、视差贴图、（选）延迟渲染 | 4~5 周 |
| 4 | 引擎架构成型 | 待开 | CMake、ECS、事件、平台抽象、RHI 雏形、游戏循环 | 4~6 周 |
| 5 | 规模化渲染与表现 | 待开 | Instancing、文字、粒子、骨骼动画 | 3~5 周 |
| 6 | 物理与碰撞 | 待开 | 自写 AABB/球/射线 → 接入 Bullet/Jolt | 2~4 周 |
| 7 | 运行时系统 | 待开 | 序列化、脚本组件、音频、生命周期 | 3~5 周 |
| 8 | 编辑器 | 待开 | ImGui、Inspector、Gizmo、Play/Stop | 4~6 周 |
| 9 | 毕业游戏 | 待开 | 用自己的引擎做一个完整小游戏 | 3~5 周 |
| 10 | 打磨与进阶 | 长期 | 性能、多线程、Android GLES、Vulkan | 按需 |

**为什么渲染（3）排在架构（4）前面**：渲染课内容小步快跑、每课都有画面反馈，
趁热打铁把 learnopengl 主线走完；ECS 是伤筋动骨的大手术，适合在"要开始堆
游戏内容（阶段 5~9）之前"集中做。但阶段 4 里的 CMake、RenderDoc 等轻量项
会在渲染线中途穿插提前做掉。

---

## 阶段 0：OpenGL 入门 ✅

对应 learnopengl「入门」全章。

- [x] GLFW 窗口 + GLAD 加载器 + 渲染循环；OpenGL 3.3 core profile
- [x] VAO / VBO / EBO、索引绘制、画第一个三角形
- [x] 着色器编写编译、uniform 传递
- [x] 纹理：stb_image、mipmap、多纹理单元
- [x] 变换矩阵、坐标系统（local→world→view→clip）
- [x] 相机：鼠标 yaw/pitch 视角 + 滚轮 fov 缩放

## 阶段 1：光照与引擎雏形 ✅

对应 learnopengl「光照」「模型加载」；架构思想开始对标商用引擎（GAMES104 分层架构）。

- [x] 完整 Blinn-Phong：环境光 / 漫反射 / 高光、点光距离衰减、聚光软边
- [x] 光源组件化：统一 LightComponent（点光 / 聚光 / 阴影灯，按字段分流）
- [x] 架构雏形：Scene / SceneObject / Component，Transform 生成 model 矩阵
- [x] 材质 JSON 数据驱动（Material::CreateFromJson，shader 路径+参数+纹理全配置）
- [x] ResourceLib 资源缓存：借用语义、永生缓存、失败资源不进缓存
- [x] 输入系统：Input（GLFW 回调层）→ ActionMap（动作映射层）；
      电平 IsDown / 边沿 WasPressed 两套查询及其时序（快照先于事件）
- [x] 手写 OBJ 加载器：扇形三角化、非法格式拒绝、范围检查
- [x] 工程地基：LOG 宏、UTF-8 编译、路径统一 getAssetPath()

## 阶段 2：高级渲染 I ✅

对应 learnopengl「高级 OpenGL」前半 + 阴影章节。

- [x] 深度测试：原理、GL_LESS/LEQUAL 语义、状态管理
- [x] 面剔除与三角形绕序契约（叉积验朝向，"剔除是照妖镜"）
- [x] 阴影贴图：light space 矩阵（灯视角正交投影+lookAt）、深度 pass、
      主 pass 采样比较；抗痤疮三件套（slope bias / 深度 pass 剔正面 / normal offset）；PCF 3×3 软边
- [x] 天空盒：cubemap 六面加载、方向向量采样、shader 内 mat3(view) 去平移、
      pos.xyww 钉深度省填充率、GL_LEQUAL + cull front
- [x] HDR 全景图 → cubemap：stbi_loadf 浮点加载、GL_RGB16F、
      util_equirect2cube 转换 shader、TextureCube::CreateFromEquirect 静态工厂、按扩展名分流
- [x] 法线贴图：切线空间理论、TBN 矩阵、Gram-Schmidt 扳正；
      顶点格式 8→11 float 加切线（程序化 mesh 解析式 / OBJ 用 Lengyel UV 差分公式）
- [x] lit uber shader：全部材质合成一个 shader（albedoMode / normalMapEnabled uniform 开关）；
      变体系统等开关集合定型后另立项
- [x] 帧缓冲后处理：Framebuffer 离屏渲染目标（颜色纹理+深度 renderbuffer、
      resize 自动重建、blit）；post 全屏 quad 透传；效果开关（灰度 / 反色 / 3×3 卷积锐化）

**验收回顾**：场景有 HDR 天空盒、地面投影、砖纹假凹凸球、F1~F3 后处理切换 ✅

## 阶段 3：高级渲染 II ← 当前阶段

对应 learnopengl「高级 OpenGL」后半 +「PBR」全部；同步看 GAMES104 渲染系统部分
（渲染数据如何组织、材质与管线、Forward vs Deferred、可见性剔除），建立"为什么"层面的认知。

- [ ] **HDR + tone mapping + gamma 校正**：RT 换 RGBA16F、Reinhard/ACES、
      线性空间→sRGB 输出；PBR 的前置（PBR 输出线性 HDR，必须配色调映射）
- [ ] **泛光 bloom**：亮度提取 → 高斯模糊 → 叠加；ping-pong 双 RT pass 管理
      实战首秀（合shader 方案在此到极限，引出"明确管理 pass"的架构）
- [ ] **PBR 理论**：金属度/粗糙度工作流、Cook-Torrance BRDF、菲涅尔、能量守恒、
      与 Blinn-Phong 的本质区别（经验近似 → 物理量）
- [ ] **PBR 直接光照**：lit shader 升级 PBR 版；Material 加 metallic/roughness；
      材质 JSON 扩展；Poly Haven 免费 PBR 材质素材
- [ ] **IBL 基于图像的光照**：辐照度图 + 预滤波环境映射 + BRDF LUT，
      环境光从天空盒采样（金属反射天空的"高级感"来源）
- [ ] **视差贴图**：法线贴图续集，bricks2_disp 素材已备
- [ ] （选）几何着色器、MSAA、SSAO、延迟渲染认知课

**预估**：4~5 周。**验收**：金属球反射天空、塑料/金属/粗糙材质一眼可辨、
发光球带泛光、砖墙有视差深度感。

## 阶段 4：引擎架构成型（大手术）★

从"组件雏形"升级成"数据驱动的引擎结构"。对应 GAMES104 分层架构 / 资源管理 / ECS；
实作对照 Hazel 前 60 集。**阶段 4 里的轻量项（CMake、RenderDoc）可在渲染线中途穿插提前做。**

- [ ] **CMake 迁移**：engine 编成静态库 + sandbox 链接（Android GLES 移植的必经路，越晚迁越痛）
- [ ] **RenderDoc 抓帧教学**：以后一切图形 bug 的第一工具
- [ ] **ECS 落地**：EnTT 上手，或照原理手写简化版（学得更深）；
      拆掉 modelObj/cubeModel 等继承子类，实体=ID、组件=数据、系统=逻辑
- [ ] **事件系统**：简单事件总线（WindowResize / KeyPressed…），发布-订阅
- [ ] **窗口/平台层抽象**：Window 基类 + 平台实现，引擎代码不再直接碰 GLFW
- [ ] **渲染器接口化**：RendererAPI 抽象（Init/Clear/DrawIndexed…），OpenGL 只是后端之一
      ——商用引擎 RHI 思想的雏形，也是将来 Vulkan 的门票
- [ ] **游戏循环**：固定步长逻辑 + 可变步长渲染（accumulator 模式，为物理预备）
- [ ] **资源系统升级**：句柄 + 引用计数，告别"永生缓存"
- [ ] **main.cpp 瘦身到 100 行内**：场景全部用「实体+组件+数据」搭出来

**预估**：4~6 周。**验收**：新增一种物体（如旋转风车）只需写一个组件类 +
场景配置加一行，不碰任何引擎代码。

## 阶段 5：规模化渲染与表现

对应 learnopengl「Instancing」「文字渲染」「Breakout 粒子」；GAMES104 动画系统两讲。

- [ ] **Instancing**：一帧画 10 万个实例，理解 drawcall 开销与合批的必要性
- [ ] **文字渲染（FreeType）**：FPS 计数、调试文本上屏
- [ ] **CPU 粒子系统组件化**：发射器 / 生命周期 / 重力，渲染走 instancing
- [ ] （选 +3 周）**骨骼动画**：glTF + assimp 导入、骨骼蒙皮、动画状态机、
      走/跑混合（GAMES104 动画系统理论同步）

**预估**：3~5 周。**验收**：发光泛光、10 万草随风摆、屏幕角 FPS、篝火粒子；
（选）角色切走路/跑步动画。

## 阶段 6：物理与碰撞

对应 GAMES104 物理系统两讲；参考 Randy Gaul《How to Create a Custom Physics Engine》。

- [ ] **自写基础版**（必做，理解原理）：AABB / 球体碰撞检测、位置修正 + 简单冲量响应、
      射线检测（编辑器点选的基础）
- [ ] **接入成熟库**（Bullet 或 Jolt）：RigidBody、Box/SphereCollider 组件、
      物理固定步长接入游戏循环

**预估**：2~4 周。**验收**：球从空中落到棋盘地面弹两下滚走；鼠标点击能拾取物体。

## 阶段 7：运行时系统

对应 GAMES104 游戏逻辑 / 平台层部分。

- [ ] **场景序列化**：实体 + 全部组件存 JSON（nlohmann/json），启动加载完整还原
- [ ] **脚本组件**：先 native C++（onCreate/onUpdate/onDestroy 虚函数），
      进阶再上 Lua（sol2）
- [ ] **音频**：miniaudio（单头文件），AudioSource / AudioListener 组件、3D 声音衰减
- [ ] **对象生命周期规范化**：延迟销毁队列（不能在遍历中直接 delete）

**预估**：3~5 周。**验收**：编辑场景→存 json→删掉代码里搭场景的部分→从 json
完整还原；碰到金币播放音效。

**★ MVP 达成线**：到这里已有「数据驱动 + 物理 + 存档 + 音频」的可用引擎，
可跳过 8、9 直接进 10，或继续冲完整版。

## 阶段 8：编辑器（商用引擎的灵魂）

对应 GAMES104 世界编辑器课；参考 Hazel 编辑器部分。

- [ ] **Dear ImGui 接入**：停靠布局，作为引擎的一个 Layer（编辑器与游戏同进程不同层）
- [ ] **Hierarchy 面板**（实体树）+ **Inspector 面板**（查看/编辑选中实体全部组件）
- [ ] **ImGuizmo**：视口平移/旋转/缩放手柄，配合射线拾取点选
- [ ] **资产浏览器**：浏览 assets/，拖拽贴图到材质
- [ ] **Play/Stop 模式**：进播放前序列化场景快照，停止还原（商用引擎同款思路）

**预估**：4~6 周。**验收**：纯鼠标操作，从空场景拼出「地面+树+光源+玩家」并 Play。

## 阶段 9：毕业游戏

引擎的毕业考试，选题：3D 滚球收集金币（Unity 经典教程同款，覆盖面最广）。

- [ ] 玩家控制（物理+输入+相机跟随）、金币收集判定、计分 HUD（文字渲染）
- [ ] 主菜单 / 暂停 / 胜负界面、BGM 音效、重新开始
- [ ] 关卡数据全放 json（验证序列化管线）
- [ ] Release 打包：exe + 资源目录，发给不懂编程的朋友能双击就玩

**预估**：3~5 周。

## 阶段 10：打磨与进阶（长期，按需选做）

- [ ] **性能**：批量渲染合批、视锥剔除、Tracy/Optick profiler、GPU 计时查询
- [ ] **渲染架构深化**：Render Graph、GPU-Driven 渲染概念（GAMES104 渲染前沿两讲）
- [ ] **多线程**：后台线程加载资源
- [ ] **Android GLES 3.3 移植**（原始目标）：Platform 层换 Android + EGL，
      CMake 在阶段 4 已备好；glad-generator 重出 GLES 加载器
- [ ] （选）网络同步；（选）Vulkan——有了自己的 RendererAPI 接口迁移才可能
- [ ] **理论补强**：GAMES104 全课、《Game Engine Architecture》、读 Piccolo 源码

---

## 贯穿始终的习惯

1. **每个功能一个 git commit**，阶段结束打 tag——引擎是长跑，历史就是保险。
2. **图形 bug 先开 RenderDoc 抓帧再猜**：看 drawcall、看绑定状态，一抓一个准。
3. **每阶段收尾跑一个 demo**，截图 + 三行说明进 CHANGELOG.md，激励自己。
4. **设计拿不准：看 Hazel / Piccolo 怎么做**，理解后用自己的方式写，不照抄。
5. **学习轨迹记 learnLog.md「## 日志」**：每完成一课追加一行「YYYY-M-D」；
   调试纠错过程不记。
