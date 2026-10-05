# 学习日志

> 2026-9-25 开始记录。开头一批是**补记**：把开始记录之前的学习内容按上下文还原（日期按 git 提交与当时记录近似）。写法：一行只写学了什么（主题 / 理论 / 方案），不写代码实现细节；调试过程不记。

## 记录前的内容（补记）

「2026-9-20」首个 OpenGL demo：GLFW 窗口 + 画三角形；键盘 IO 分离、渲染分离。

「2026-9-20」顶点 / 索引 / VAO-VBO-EBO；shader 编译链接流程；纹理加载与 mipmap。

「2026-9-21」渲染与对象管理解耦：Scene / SceneObject / Component 结构、MeshRenderer 组件、顶点材质复用。

「2026-9-21」鼠标相机（yaw / pitch / 滚轮 fov）；完整 Blinn-Phong 光照：环境光、漫反射、高光、点光距离衰减、聚光软边。

「2026-9-22」材质 JSON 数据驱动；资源缓存机制：借用语义、永生缓存、失败资源不进缓存。

「2026-9-22」Shader 按模块拆分：共享光照块 + #include 预处理；自写 OBJ 加载器：扇形三角化、非法格式拒绝。

「2026-9-24」深度贴图 pass + 可视化；light space 矩阵：灯视角的正交投影 + lookAt。

「2026-9-24」主 pass 采样比较出影子；抗痤疮三件套：slope bias、深度 pass 只画背面、normal offset。

「2026-9-25」PCF 3×3 软边，阴影 shadow mapping 主线收官。

「2026-9-25」天空盒：cubemap 六面加载、方向向量采样；三技巧：视图去平移、钉死深度、LEQUAL + 剔正面。

「2026-9-25」主 pass 开启面剔除；三角形绕序契约：叉积验朝向。

「2026-9-25」材质 shader 合并归一：uniform 开关先行，变体系统等开关定型再上。

「2026-9-25」法线贴图理论：光照只认法线 → 用贴图存假法线骗光照；切线空间（表面局部坐标系 / 贴纸类比）、TBN 矩阵、RGB→[-1,1]。

「2026-9-25」切线数据：程序化网格解析式求解；OBJ 用 Lengyel 公式（UV 差分）数值计算。

「2026-9-25」法线贴图接入：TBN 变换 + 材质开关；球面砖纹假凹凸验收通过。

「2026-9-25」shader 组织观：管线 / 工具 pass shader 独立成件（Unity Hidden/ 模式），材质 shader 靠 uber-shader + 变体 + 共享 include 控膨胀；数 shader 按 pass 类型数，不按功能数。

「2026-9-25」HDR 全景图接入：等距全景图离线转 cubemap（静态工厂 + 按扩展名分流）；Poly Haven 4k 全景素材入库。

## 日志

「2026-9-26」HDR 天空盒：全景图转 cubemap 管线验收。

「2026-9-26」帧缓冲后处理：离屏渲染 Framebuffer + 全屏 quad 透传 + 效果开关（灰度/反色/锐化），主线完成。

「2026-9-26」HDR/泛光：浮点画布、tone mapping、bloom 三段管线（亮度提取/高斯模糊 ping-pong/合成）、gamma 校正；环境光归场景级、sRGB 输入线性化。

「2026-9-27」RenderDoc 抓帧：挂接捕获、事件/纹理/管线三视图入门，draw call 对账（25 次）与 HDR 浮点数值验证。

「2026-9-27」PBR 直接光照：Cook-Torrance 三项（GGX 法线分布/菲涅尔/Smith 几何）+ metallic/roughness 工作流，金属/非金属对比球阵；HDR 高光出口钳与欠采样调参。

「2026-9-27」PBR 材质贴图化：normal/metallic/roughness 贴图接入 PBR（出现即启用、3/4 号槽位），Poly Haven 金属板/瓷砖素材球验收通过，PBR 主线完结。

「2026-10-1」IBL 环境光照三部曲：辐照度图（漫反射半球卷积）、GGX 重要性采样预滤波图（镜面按粗糙度分 mip 层）、BRDF LUT（split-sum 材质响应查表）；环境光从常数升级为真实环境响应，金属球镜面复活验收。

「2026-10-1」抗闪烁调优：bloom 逐样本能量钳位、屏幕空间法线导数动态粗糙度下限（fwidth 方差补偿，自研）；cubemap 无缝过滤。

「2026-10-2」灯光语义统一课：阴影归位只乘投影灯、PBR 接入聚光锥与阴影采样（calcShadow 上提 common.glsl）、range 衰减窗口消除硬截断跳变；能量来源辨析（直射光 vs IBL 环境光、调制因子分层）。

「2026-10-2」bloom 金字塔重构：全分辨率多遍 ping-pong 换降采样塔（box 盒滤波）+ tent 上采样 additive 合成；踩 viewport 遗留坑（pass 自设状态原则）、反馈环（读写同纹理→additive blend 解法）、金字塔低频能量倍增（天空变亮→软阈值 knee 收紧提取）、降采样马赛克锯齿（九点盒）。

「2026-10-3」AO 贴图与 ORM 三合一打包：环境光遮蔽只调环境光（可见度=环境光占比辨析）、glTF 通道约定实测与资产通道搬运（Poly Haven 反序陷阱）、材质参数三级优先级瀑布（标量→solo 贴图→ORM）、变体系统动机预热（sampler 槽位硬上限 vs 编译期裁剪）。

「2026-10-3」F1 目录重组：Client 散文件按模块归位 engine/{core,platform,render,scene}+game；文件名对齐主类名（sys→Paths、camera→CameraComponent 等）；Scene.h 六类拆分（SceneObject/LightComponent 独立成件）；include 全量模块前缀化；vcxproj+filters 同步。

「2026-10-3」F2 引擎/游戏分离：engine 编静态库 + game 项目引用（构建顺序与链接自动管理，glad.c 归引擎）；main.cpp 瘦身至 85 行，场景搭建与输入映射归位 Game.cpp；processInput 出 platform（游戏玩法逻辑不进引擎层，KeyBoard 文件退役）；CRT 运行库对齐教训（Debug /MD 与 GLFW 预编译库一致，两个项目必须同配）。

「2026-10-3」F3-1 Window 类：引擎代码不再直接碰 GLFW——构造一条龙（init/hint/create/makeCurrent/glad 加载）+ 前置声明 GLFWwindow 头文件零泄漏 + user pointer 转发成员回调 + SetResizeCallback 即触发初始 resize；顺手修 scene 层直调 glfwGetTime 的依赖违规（新 platform/Time::Now 收编）；main.cpp 85→62 行，窗口生死归 Window 析构。

「2026-10-3」F3-2 RendererAPI 接口：GL 状态命令收拢为抽象接口——自定义 CullMode/DepthFunc/BlendFactor 词汇表（引擎词≠API 词，翻译只活在实现里）、Create 工厂集中选择点、OpenGLRendererAPI 后端实现；Renderer.cpp 状态直调清零；踩坑：换行时复制忘改函数名（SetBlend 写成 SetDepthTest，blend 没开泛光变弱）+ 纯虚声明漏实现到链接期才炸（编译查声明、链接查地址教案）。

「2026-10-3」F3-3a Mesh/Shader 资源接口化：抽象基类+OpenGL 后端实现+静态工厂三件套；设计三原则（头文件零 GL 连句柄不露、顶点数学公共层与 GPU 上传后端分离、loc 缓存藏后端私有）；52 处 glUniform 直调机械替换清零、glUniform 全工程只剩后端实现；抽象副产品=编译器抓出两处外部摸内部成员（ResourceLib 的 m_valid→IsValid 接口、日志的 vertexCount→VertexCount 接口）。

「2026-10-3」F3-3b 纹理/渲染目标接口化：Texture 抽象基类（Bind(unit) 统一 2D/CUBE 目标语义）+ Texture2D/TextureCube 后端实现；句柄流转改对象流转（EnvFrame/ShadowFrame/Material 全部存资源指针，借用语义真落地）；烘焙工厂对象传参+CreateBrdfLut 进后端；Framebuffer 抽象吸收 ShadowMap（depthOnly 模式，深度附件可采样）；Shutdown 裸 glDeleteTextures 改 unique_ptr 所有权闭环。踩坑案：①重写 GetTexture 丢 getAssetPath 拼接→所有 2D 贴图加载失败→症状分布定位法（纯标量材质的球阵亮、带贴图的全黑=Texture2D 链路）；②friend 授权抽象类静态工厂访问后端私有构造；③GL 对象生命周期≠C++ 变量（RBO 局部变量泄漏用户自己抓出）；④旧实现文件（Framebuffer.cpp）未删与新抽象头冲突。

「2026-10-3」F3 收官（F3-3c 降级为依赖审计）：全工程审计证明 scene/game 层 OpenGL 直调清零、组件只依赖渲染抽象接口（换 Vulkan 后端组件零改动目标达成）；render→scene 仅剩数据消费（Renderer 读场景），与 UE 模块结构同构，RenderScene 代理手术判定为过度工程、登记长期债务（render graph 时解）；main.cpp 尾巴 glViewport 清除=游戏层彻底零 GL。F3 四层成果：模块目录边界、Window 平台抽象、RendererAPI 命令接口、Mesh/Shader/Texture/Framebuffer 资源接口化。

「2026-10-3」F4 游戏循环：accumulator 模式固定步长逻辑（FIXED_DT=1/60）+ 可变步长渲染彻底解耦；防死亡螺旋钳制（帧间隔上限 0.25s）；输入视角保持真实帧间隔——游戏状态要确定性（回放/联机/物理稳定）、输入要手感，两层身份不同的商用分工（Unity Update vs FixedUpdate 同构）；验收=FIXED_DT 调 0.1 实验：旋转 10Hz 卡顿慢放但转速正确、相机依旧丝滑。
