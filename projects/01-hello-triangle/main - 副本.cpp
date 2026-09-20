// ============================================================================
// 01-hello-triangle —— 你的第一个 OpenGL 程序（学习版：每个函数都有详细注释）
//
// 效果：弹出一个 800x600 窗口，画一个彩色三角形，三角形会左右轻轻摆动
// 操作：按 ESC 退出
//
// 对应教程：learnopengl.com 《Getting Started -> Hello Triangle》
// 中文版： https://learnopengl-cn.github.io/01%20Getting%20started/04%20Hello%20Triangle/
//
// 代码分五步，每步都用 "---------- 数字 ----------" 标出：
//   1. 初始化 GLFW 并创建窗口
//   2. 用 GLAD 加载 OpenGL 函数指针
//   3. 编写着色器(Shader) + 上传顶点数据(VBO/VAO)
//   4. 渲染循环
//   5. 退出前释放资源
//
// 本文件用到的函数速查表（往下读遇到不懂的函数，也可以 Ctrl+F 回来搜）：
//   【窗口/输入 GLFW】glfwInit  glfwWindowHint  glfwCreateWindow  glfwTerminate
//      glfwMakeContextCurrent  glfwSwapInterval  glfwSetFramebufferSizeCallback
//      glfwGetProcAddress  glfwGetTime  glfwWindowShouldClose  glfwGetKey
//      glfwSetWindowShouldClose  glfwSwapBuffers  glfwPollEvents
//   【函数加载 GLAD】gladLoadGLLoader
//   【画图 OpenGL】
//      环境类：glViewport  glGetString
//      着色器类：glCreateShader  glShaderSource  glCompileShader  glGetShaderiv
//        glGetShaderInfoLog  glCreateProgram  glAttachShader  glLinkProgram
//        glGetProgramiv  glGetProgramInfoLog  glDeleteShader  glDeleteProgram
//        glUseProgram  glGetUniformLocation  glUniform1f
//      顶点数据类：glGenVertexArrays  glGenBuffers  glBindVertexArray
//        glBindBuffer  glBufferData  glVertexAttribPointer
//        glEnableVertexAttribArray  glDeleteVertexArrays  glDeleteBuffers
//      绘制类：glClearColor  glClear  glDrawArrays
//   【C 标准库】printf  fflush  sinf
//
// 三个库的分工（先搞清楚这个，代码就好读了）：
//   GLFW：管窗口、键盘鼠标（和 OpenGL 无关的杂活）
//   GLAD ：启动时把 OpenGL 函数的"真实地址"从显卡驱动里加载进来
//   OpenGL：真正负责画图（glXxx 开头的全是它）
// ============================================================================

#include <glad/glad.h>   // 注意：glad.h 必须写在 glfw3.h 之前
                         // （glfw3.h 内部会间接包含系统 OpenGL 头，glad 要抢先占位）
#include <GLFW/glfw3.h>  // GLFW 负责创建窗口、处理键盘鼠标输入
#include <cstdio>        // printf：往控制台打印文字
#include <cmath>         // sinf：正弦函数，用来做三角形左右摆动的动画

// ============================================================================
// 回调函数：窗口大小改变时，GLFW 会自动调用它，把 OpenGL 视口同步成新尺寸
// ----------------------------------------------------------------------------
// 这个函数不是我们手动调用的，而是"注册"给 GLFW 的（见下面 main 里
// glfwSetFramebufferSizeCallback 那行）。流程是：
//   注册 -> 用户拖动窗口边框 -> GLFW 检测到尺寸变化 -> 自动调用本函数，
//   并把"新宽高"作为参数传进来。
// 为什么要这么设计：我们无法预知用户什么时候拖窗口，只能把处理逻辑写好交给
//   GLFW，它发现变化时替我们调用。这就是"事件驱动/回调"模式。
// ============================================================================
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // ◆ glViewport(左下角x, 左下角y, 宽, 高)
    //   功能：告诉 OpenGL，渲染出来的画面要贴到窗口的哪一块矩形区域里。
    //         前两个参数是这块区域"左下角"的坐标，后两个是宽高（单位都是像素）。
    //         注意：OpenGL 的窗口坐标原点在"左下角"，和 Windows/屏幕的"左上角"
    //         原点方向相反，这是新手常踩的坑。
    //   为什么传 (0,0,width,height)：让画面铺满整个窗口客户区（不含标题栏）。
    //   为什么宽高用参数而不是写死 800/600：回调传来的就是变化后的最新尺寸，
    //         用参数值画面才能跟着窗口实时缩放。
    glViewport(0, 0, width, height);
}

// ============================================================================
// 处理输入：渲染循环里每帧调用一次，检查键盘状态
// ----------------------------------------------------------------------------
// 把"读输入"单独包成函数是代码组织习惯：渲染循环主流程保持干净——
//   处理输入 -> 渲染 -> 交换缓冲 -> 处理事件。
// 以后加 WASD 移动相机等逻辑都写在这里，不用到处翻 main。
// ============================================================================
void processInput(GLFWwindow* window) {
    // ◆ glfwGetKey(窗口句柄, 键码)
    //   功能：查询某个键"此刻"的状态，返回 GLFW_PRESS(被按住) 或
    //         GLFW_RELEASE(松开)。GLFW_KEY_ESCAPE 是 GLFW 定义好的 ESC 键码。
    //   为什么用主动查询而不是按键回调：本项目只关心"这一帧 ESC 是否被按住"
    //         这种持续状态，每帧查一次最简单直观；要响应"单击瞬间"才用回调。
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        // ◆ glfwSetWindowShouldClose(窗口句柄, true)
        //   功能：把窗口内部的"应该关闭"标志置为 true。
        //   为什么不直接关窗口：GLFW 把关闭的"决定权"留给程序。它只设标志，
        //         下一轮 while 里 glfwWindowShouldClose() 读到 true，循环自然结束，
        //         我们就能顺序走完清理资源的代码再退出，而不是被粗暴掐断。
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // ==================== 1. 初始化 GLFW，创建窗口 ====================

    // ◆ glfwInit()
    //   功能：初始化 GLFW 库，准备窗口系统需要的内部资源（Windows 上会做
    //         注册窗口类之类的准备工作）。
    //   为什么必须是第一行：GLFW 规定其他所有 glfwXxx 函数必须在 glfwInit()
    //         成功之后才能调用，否则行为未定义（很可能直接崩溃）。
    //   返回值：GLFW_TRUE 成功 / GLFW_FALSE 失败。正式项目应检查返回值，
    //         教程代码常省略，这里从简。
    glfwInit();

    // ◆ glfwWindowHint(选项名, 选项值)
    //   功能：设置"下一个要创建的窗口"的属性。只对它之后创建的第一个窗口生效，
    //         窗口一创建，这组设置就被"消费"掉了，所以必须写在 glfwCreateWindow 之前。
    //   为什么需要它：OpenGL 上下文有很多版本和模式，不提前声明，驱动可能给一个
    //         老版本或兼容模式的上下文，导致后面 GLSL 着色器编译不过。
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // 告诉 GLFW 我们要
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);  // OpenGL 3.3（主版本 3 + 次版本 3）
    // GLFW_OPENGL_PROFILE：上下文的"功能画像"。
    // GLFW_OPENGL_CORE_PROFILE = 核心模式：只提供现代的可编程管线，砍掉了
    //   glBegin/glEnd 那套上个时代的固定管线 API。学现代 OpenGL 就用它；
    //   macOS 只支持核心模式，所以跨平台代码几乎都这样写。
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // ◆ glfwCreateWindow(宽, 高, 标题, 显示器, 共享资源的窗口)
    //   功能：创建一个窗口，并顺带创建与之绑定的 OpenGL 上下文(context)。
    //         上下文 = "OpenGL 的全部状态 + 能干活的环境"，没有它任何 glXxx
    //         函数都无法工作。
    //   参数：第 4 个 monitor 传 NULL 表示普通窗口（传显示器句柄就变全屏）；
    //         第 5 个传 NULL 表示不与其他窗口共享纹理/缓冲等资源。
    //   返回值：窗口句柄 GLFWwindow*，之后所有 glfw 函数靠它指定"操作哪个窗口"。
    //         失败返回 NULL（典型原因：显卡驱动不支持我们要的 3.3 核心模式）。
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL: Hello Triangle", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        // ◆ glfwTerminate()
        //   功能：清理并关闭 GLFW，释放它占用的一切资源。
        //   为什么在这里调用：glfwInit 已经执行过，程序现在要退出，必须把 GLFW
        //         收尾干净。它和 glfwInit() 成对出现：init 成功后，所有退出路径
        //         都应该 terminate 一次。
        glfwTerminate();
        return -1;
    }

    // ◆ glfwMakeContextCurrent(窗口句柄)
    //   功能：把这个窗口的 OpenGL 上下文设为"当前线程的上下文"。
    //   为什么必须调用：OpenGL 是个状态机，之后所有 glXxx 调用都作用于
    //         "当前上下文"。创建了窗口不调用这句，后面任何 gl 函数都会无效甚至崩溃。
    //   注意"当前线程"：上下文是绑定线程的，本项目只在主线程用，够用。
    glfwMakeContextCurrent(window);      // 把这个窗口设为当前上下文

    // ◆ glfwSwapInterval(1)
    //   功能：设置交换缓冲的间隔。参数 1 = 每 1 次显示器刷新才交换一次画面
    //         （即开启垂直同步 VSync）。
    //   为什么这么用：60Hz 显示器下若不开垂直同步，渲染循环一秒能空转几千圈，
    //         CPU/GPU 空耗、风扇狂转，画面还可能撕裂；设为 1 后帧率被锁定在
    //         显示器刷新率，省电又稳定。
    //   前提：必须在 glfwMakeContextCurrent 之后调用才生效。
    glfwSwapInterval(1);                 // 开启垂直同步，防止占满 CPU/GPU

    // ◆ glfwSetFramebufferSizeCallback(窗口句柄, 回调函数名)
    //   功能：注册"窗口尺寸变化"的回调函数。注册后，窗口大小每次变化 GLFW
    //         都会自动调用它（包括用户拖边框、最大化/还原窗口）。
    //   为什么这么用：回调模式——处理逻辑写在上面 framebuffer_size_callback 里，
    //         这里只是告诉 GLFW"尺寸变了请调用它"。传的是函数名（函数指针）。
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ================== 2. 用 GLAD 加载 OpenGL 函数指针 ==================
    //
    // 背景：OpenGL 的函数地址没有固定写死在系统库里——不同驱动、不同版本，
    // 地址都不一样。现代做法是运行时拿着"函数名"去显卡驱动那里换"函数地址"。
    //
    // ◆ glfwGetProcAddress("函数名")
    //   功能：向驱动查询某个 OpenGL 函数的地址（参数是函数名字符串）。
    //   为什么出现在这里：它是 GLAD 加载函数要用的"查询工具"。GLAD 会拿着
    //         函数名一个个来问它："glViewport 的地址是多少？glClear 呢？"
    //
    // ◆ gladLoadGLLoader(查询函数)
    //   功能：GLAD 一次性加载全部 OpenGL 函数指针。传入一个查询函数，
    //         它内部用这个函数把 glad.h 里声明的所有 gl 函数的真实地址填好。
    //   为什么必须调用：glad.h 里的函数指针一开始全是空的，不加载就调用
    //         直接崩溃。返回 0 = 失败，非 0 = 成功。
    //   为什么必须放在 glfwMakeContextCurrent 之后、所有 glXxx 调用之前：
    //         前者因为"查函数地址"必须先有当前上下文；后者不解释，顺序就这三步：
    //         创建窗口 -> MakeContextCurrent -> gladLoadGLLoader。
    //   (GLADloadproc) 是强制类型转换：把 glfwGetProcAddress 转成 GLAD 期望的
    //         函数指针类型（两者签名兼容，转一下让编译器闭嘴）。
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    // 打印当前环境信息：看看你的显卡驱动到底提供了哪个版本的 OpenGL
    //
    // ◆ glGetString(枚举值)
    //   功能：返回一项 OpenGL 环境信息的字符串。常用枚举：
    //         GL_VERSION = OpenGL 版本号；GL_RENDERER = 显卡型号（渲染器名）；
    //         GL_VENDOR = 驱动厂商；GL_SHADING_LANGUAGE_VERSION = GLSL 版本。
    //   返回类型是 const GLubyte*（即 unsigned char*），所以要强转成
    //   (const char*) 才能交给 printf 的 %s。
    //   为什么这么用：程序跑不起来时，第一件事就是看这几行输出，确认驱动
    //         真的给了 3.3、跑在独立显卡而不是集显上。
    printf("OpenGL version : %s\n", (const char*)glGetString(GL_VERSION));
    printf("GPU renderer   : %s\n", (const char*)glGetString(GL_RENDERER));
    printf("Press ESC to quit.\n\n");

    // ◆ fflush(stdout)
    //   功能：强制把标准输出缓冲区里的内容立刻刷到屏幕上。
    //   为什么这么用：printf 的文字先攒在缓冲区里，攒够一批才真正显示。
    //         Windows 控制台下若不主动刷，这几行可能要等程序退出才冒出来，
    //         调试时想"马上看到"，就在打印后手动 fflush 一次。
    fflush(stdout);  // 立刻把上面几行显示到控制台

    // ◆ glViewport —— 参数含义见上面回调函数里的详细注释。
    //   为什么这里还要再调一次：回调只在"尺寸发生变化时"触发；程序刚启动时
    //   窗口已经是 800x600，但没有发生"变化"，回调不会响，所以要手动设一次
    //   初始视口。（设了这句，就算窗口一直不被拖动，画面位置也是对的。）
    glViewport(0, 0, 800, 600);  // 告诉 OpenGL 窗口哪块区域用来渲染

    // ============ 3. 着色器 + 顶点数据（可编程管线的核心概念） ============
    //
    // 先背下这个整体流程，它和后面 VBO/VAO 的套路一模一样：
    //   写源码(字符串) -> glCreateShader 创建对象 -> glShaderSource 喂源码
    //   -> glCompileShader 编译 -> glGetShaderiv 检查是否成功
    //   （对第二个着色器重复一遍）
    //   -> glCreateProgram 建程序 -> glAttachShader 把两个着色器挂上
    //   -> glLinkProgram 链接 -> 检查链接结果 -> glDeleteShader 删中间产物

    // ---- 3.1 顶点着色器：GPU 上运行，负责处理每一个顶点 ----
    // 输入 3 个顶点位置，输出到裁剪空间；xOffset 是每帧传入的偏移量(动画)
    // 源码是 GLSL 语言（类 C），以字符串形式写在程序里，运行时才交给驱动编译。
    const char* vertexShaderSource = R"(#version 330 core
layout (location = 0) in vec3 aPos;     // 位置属性，location=0
layout (location = 1) in vec3 aColor;   // 颜色属性，location=1
uniform float xOffset;                  // uniform: 每帧由 CPU 传进来的全局变量
out vec3 ourColor;                      // 传给片段着色器的插值颜色
void main() {
    gl_Position = vec4(aPos.x + xOffset, aPos.y, aPos.z, 1.0);
    ourColor = aColor;
})";
    // 上面 GLSL 里几个关键字的含义（配合后面 glVertexAttribPointer 看）：
    //   in vec3 aPos   ：顶点着色器的输入，每个顶点喂一次；location=0 是它的"插槽号"
    //   uniform        ：所有顶点共享的全局变量，由 CPU 每帧设置（动画参数走这里）
    //   out vec3       ：输出给下一个阶段（片段着色器）的数据
    //   gl_Position    ：GLSL 内置变量，顶点的最终位置（裁剪空间坐标），必须赋值

    // ---- 3.2 片段着色器：决定屏幕上每个像素最终是什么颜色 ----
    const char* fragmentShaderSource = R"(#version 330 core
in vec3 ourColor;    // 从顶点着色器插值而来
out vec4 FragColor;  // 输出的像素颜色
void main() {
    FragColor = vec4(ourColor, 1.0);
})";
    // in vec3 ourColor：三个顶点各带一个颜色，GPU 会自动在三角形表面上做
    //   "插值"——所以三角形内部呈现三色渐变。最后 1.0 是不透明度 alpha。

    // ◆ glCreateShader(着色器类型)
    //   功能：创建一个空的着色器对象，返回它的 ID（句柄）。
    //   参数：GL_VERTEX_SHADER = 顶点着色器；GL_FRAGMENT_SHADER = 片段着色器
    //         （还有几何着色器等类型，本项目只用到前两种）。
    //   为什么返回 ID 而不是对象本身：OpenGL 是 C API，所有"对象"（着色器/缓冲/
    //         纹理）都用一个 unsigned int 代表，之后靠这个 ID 引用对象——这是
    //         OpenGL 的统一风格，和 glGenBuffers 返回 ID 是一个道理。
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // ◆ glShaderSource(着色器ID, 字符串个数, 字符串指针数组, 长度数组或NULL)
    //   功能：把 GLSL 源码字符串交给着色器对象（只是存起来，还没编译）。
    //   参数细节：第 3 个参数是 const char**，因为允许一次传多个字符串拼成一份
    //         源码；我们只有 1 个字符串，所以写 &vertexShaderSource（取地址得到
    //         二级指针）。第 4 个参数传 NULL 表示"以 \0 结尾，长度你自己数"。
    //   为什么源码是字符串：着色器在 GPU 上运行，编译发生在"程序运行时"，
    //         所以源码只能以字符串形式随身携带。
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

    // ◆ glCompileShader(着色器ID)
    //   功能：让驱动编译刚喂进去的源码（GLSL -> GPU 机器码）。
    //   为什么没有返回值：OpenGL 大量函数不直接返回结果，结果要"事后查询"——
    //         所以下一行紧跟 glGetShaderiv 检查。这种"先调用、再查询"的模式
    //         在 OpenGL 里无处不在，习惯它。
    glCompileShader(vertexShader);

    // ◆ glGetShaderiv(着色器ID, 查什么, 结果写到哪)
    //   功能：查询着色器对象的信息，结果写入最后一个 int（iv = integer value）。
    //         这里查 GL_COMPILE_STATUS：成功写入 GL_TRUE(1)，失败写入 GL_FALSE(0)。
    //         还能查 GL_SHADER_TYPE、GL_INFO_LOG_LENGTH(错误日志长度) 等。
    int success; char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        // ◆ glGetShaderInfoLog(着色器ID, 缓冲区大小, 实际长度(NULL=不关心), 日志缓冲区)
        //   功能：把编译失败的错误信息（第几行、什么错）填进 infoLog 字符串。
        //   为什么必须有它：上面的 iv 只告诉你"失败了"，不说为什么；日志会给出
        //         类似 "ERROR: 0:3: 'vec3' : syntax error" 的具体信息。写错 GLSL
        //         是家常便饭，这是调试着色器最重要的函数。
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR vertex shader:\n%s\n", infoLog); return -1;
    }

    // 片段着色器完全相同的四步：创建 -> 喂源码 -> 编译 -> 检查
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) { glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR fragment shader:\n%s\n", infoLog); return -1; }

    // ◆ glCreateProgram()
    //   功能：创建一个着色器程序(program)对象，返回 ID。
    //         程序 = 若干着色器链接成的整体；渲染时真正"使用"的是程序，
    //         而不是单个着色器。
    //   为什么需要"程序"这一层：GPU 管线是一整条流水线（顶点着色 -> 光栅化 ->
    //         片段着色 -> ...）。链接时驱动会检查顶点着色器的 out 和片段着色器
    //         的 in 能否对上号，最终生成一条完整可执行的管线。
    unsigned int shaderProgram = glCreateProgram();

    // ◆ glAttachShader(程序ID, 着色器ID)
    //   功能：把编译好的着色器"挂到"程序上——相当于把零件收集进车间，还没组装。
    //   为什么这么用：至少挂"1 顶点 + 1 片段"着色器程序才完整；必须两个都挂上
    //         才能链接。挂上去之前，着色器必须已经编译成功。
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // ◆ glLinkProgram(程序ID)
    //   功能：把挂上来的着色器链接成完整程序（零件组装成整机）。
    //   为什么可能失败：两个着色器的 in/out 变量名或类型对不上、超出了硬件
    //         能力等。同样是"调用 -> 查询"模式检查结果。
    glLinkProgram(shaderProgram);

    // ◆ glGetProgramiv(程序ID, 查什么, 结果写到哪)
    //   功能：查询程序对象的信息，是 glGetShaderiv 的"程序版"。
    //         这里查 GL_LINK_STATUS：链接成功 GL_TRUE / 失败 GL_FALSE。
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        // ◆ glGetProgramInfoLog —— 用法同 glGetShaderInfoLog，只是对象换成了程序，
        //   拿到的是"链接错误"的日志。
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR link:\n%s\n", infoLog); return -1;
    }

    // ◆ glDeleteShader(着色器ID)
    //   功能：删除着色器对象，释放驱动侧的资源。
    //   为什么链接完就删：着色器只是"零件"，链接完成后程序里已经有完整成品，
    //         零件留着没用。注意：删的是着色器对象，shaderProgram 不受影响。
    glDeleteShader(vertexShader);     // 链接完成后，中间产物可以删掉
    glDeleteShader(fragmentShader);

    // ---- 3.3 三角形的顶点数据：每行 = (x,y,z) 位置 + (r,g,b) 颜色 ----
    // 坐标是 NDC(标准化设备坐标)：x、y 范围都是 [-1,1]，(0,0) 在窗口正中心。
    // 位置和颜色"交错"存在同一个数组里：每 6 个 float 是一个顶点的完整数据。
    float vertices[] = {
        //   位置              颜色
        -0.5f, -0.5f, 0.0f,  1.0f, 0.5f, 0.2f,   // 左下：橙
         0.5f, -0.5f, 0.0f,  0.2f, 0.8f, 0.3f,   // 右下：绿
         0.0f,  0.6f, 0.0f,  0.2f, 0.5f, 1.0f,   // 顶上：蓝
    };

    // ◆ glGenVertexArrays(要几个, 存ID的数组)
    //   功能：申请 1 个 VAO(顶点数组对象)的 ID，存进变量 VAO。
    //   VAO 是什么：一本"顶点数据说明书"，记录两件事：
    //     1) 启用了哪些顶点属性(位置/颜色...)、每条属性怎么从显存里读出来；
    //     2) 这些读法对应哪块 VBO 显存。
    //   为什么需要它：没有 VAO，每帧画图前都得重新执行一遍 glVertexAttribPointer
    //         等一长串设置；有了 VAO，配置一次，之后画图前一句 glBindVertexArray
    //         就全部恢复。另外核心模式下强制要求必须有 VAO 才能画图。
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);

    // ◆ glGenBuffers(要几个, 存ID的数组)
    //   功能：申请 1 个缓冲对象(VBO) 的 ID。
    //   VBO 是什么：Vertex Buffer Object，一块真正存放顶点数据的 GPU 显存。
    //   为什么叫 "Gen"(生成) 而不是 Create：历史命名——早期 OpenGL 的对象是
    //         "先发名字、第一次绑定时才真正创建"，Gen 只负责发 ID。你把它
    //         理解成"创建"即可。glGenXxx 发 ID，glCreateXxx 发 ID 且立即创建。
    glGenBuffers(1, &VBO);        // VBO 是真正存放顶点数据的显存块

    // ◆ glBindVertexArray(VAO的ID)
    //   功能：把 VAO 绑定为"当前 VAO"。之后所有顶点属性设置
    //         （glVertexAttribPointer / glEnableVertexAttribArray）都会被记录进
    //         这本"说明书"。
    //   为什么先绑 VAO：顺序很关键！必须先绑 VAO、再做属性设置，设置才会被
    //         VAO 记住。顺序反了 = 白设置。
    glBindVertexArray(VAO);

    // ◆ glBindBuffer(目标槽位, 缓冲ID)
    //   功能：把 VBO 绑定到 GL_ARRAY_BUFFER 这个"槽位"上，使它成为当前操作的缓冲。
    //   为什么要有绑定这步：OpenGL 靠"目标(target)"工作——GL_ARRAY_BUFFER 就是
    //         "顶点数据专用槽"。绑上之后，下一句 glBufferData 才知道数据该送进
    //         哪个缓冲。同一个槽位换着绑不同缓冲，就能切换操作对象。
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // ◆ glBufferData(目标槽位, 数据字节数, 数据指针, 用途提示)
    //   功能：把 CPU 内存里的数据复制一份，送进当前绑定的缓冲（GPU 显存）。
    //   参数：sizeof(vertices) 算出 3 个顶点共 72 字节；vertices 是源数据指针；
    //         GL_STATIC_DRAW 是给驱动的优化提示："这份数据写一次、画很多次"，
    //         驱动会把它放到合适的显存位置。数据每帧都变就改用 GL_DYNAMIC_DRAW。
    //   为什么是"复制"而不是引用：函数返回后，CPU 侧数组与 GPU 就无关了，
    //         显存里是独立副本；以后想改显存数据要走 glBufferSubData 等接口。
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // ◆ glVertexAttribPointer(属性location, 每顶点几个分量, 类型, 是否归一化, 步长, 起始偏移)
    //   功能：告诉 OpenGL "怎么从 VBO 那坨连续内存里，读出每个顶点的这个属性"，
    //         并把这个读法记录进当前绑定的 VAO。
    //   参数逐个看（以属性 0 = 位置为例，内存布局：x y z r g b | x y z r g b | ...）：
    //     0               -> 对应顶点着色器里 layout(location=0) 的 aPos
    //     3               -> 位置由 3 个分量组成 (x,y,z)
    //     GL_FLOAT        -> 分量是 float 类型
    //     GL_FALSE        -> 不归一化（整数数据才涉及归一化到 [0,1]，float 填它）
    //     6*sizeof(float) -> 步长 stride：相邻两个顶点的"同属性"间隔 24 字节，
    //                        也就是每次读 3 个 float 后，跳过一整个顶点(6个float)
    //     (void*)0        -> 该属性在每个顶点数据块内的起始偏移：位置排在最前，偏移 0
    //   为什么最后一个参数要写成 (void*)0：历史遗留——它不是真指针，而是"相对
    //         VBO 开头的字节偏移"，但参数类型是 void*，必须强转，否则编译报错。
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    // ◆ glEnableVertexAttribArray(属性location)
    //   功能：启用 0 号顶点属性，让 GPU 渲染时真的去读它。
    //   为什么必须有：顶点属性默认是"禁用"状态，禁用时 GPU 会拿一份固定常量
    //         值代替真实数据，画面就不对。每个用到的 location 都要 enable 一次。
    glEnableVertexAttribArray(0);

    // 属性 1 = 颜色：同样 3 个 float；偏移是 3*sizeof(float)，因为颜色排在每个
    // 顶点数据块的位置(x,y,z)后面，从第 12 个字节才开始读。
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 到这里 VAO"说明书"写完了：以后画图前只要 glBindVertexArray(VAO) 一句，
    // 全部读法自动生效。

    // ========================= 4. 渲染循环 =========================
    // 每一轮 = 检查退出 -> 读输入 -> 清屏 -> 配置着色器 -> 画 -> 显示 -> 处理事件。
    // 开了垂直同步后一秒循环约 60 次，每圈画一帧。所有游戏/图形程序都是这个骨架。

    // ◆ glfwWindowShouldClose(窗口句柄)
    //   功能：读取窗口的"应该关闭"标志，true = 该退出了。
    //   什么时候变 true：用户点了窗口 X 按钮；或我们按 ESC 时调用的
    //         glfwSetWindowShouldClose；或系统要求关闭。为 true 时 while 结束，
    //         程序进入第 5 步清理。
    while (!glfwWindowShouldClose(window)) {
        processInput(window);   // 读键盘（实现在文件开头）

        // ◆ glClearColor(红, 绿, 蓝, 透明度)，各分量范围 0.0~1.0
        //   功能：设置"清屏用什么颜色"。注意它只是改了一个状态，此刻屏幕上
        //         什么都还没发生！
        //   为什么和 glClear 分成两个函数：OpenGL 的设计哲学是"状态机"——
        //         一类函数设置状态(SetXxx/glClearColor)，另一类函数使用状态
        //         (glClear)。好处：颜色设一次以后一直有效，不必每帧重复
        //         （这里为了教学每帧都写一遍，也无妨）。
        glClearColor(0.1f, 0.12f, 0.15f, 1.0f);         // 设置清屏色(深蓝灰)

        // ◆ glClear(要清除哪些缓冲)
        //   功能：真正动手，用上面设置的颜色把颜色缓冲（当前画面）整个填一遍。
        //   参数：GL_COLOR_BUFFER_BIT = 颜色缓冲；以后画 3D 还会按位或上
        //         GL_DEPTH_BUFFER_BIT(深度缓冲)：glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)。
        //   为什么每帧都要清：不清的话上一帧的三角形还留在原地，新一帧叠上去
        //         就是满屏鬼影。
        glClear(GL_COLOR_BUFFER_BIT);                    // 用清屏色填满屏幕

        // ◆ glUseProgram(程序ID)
        //   功能：把这个着色器程序设为"当前程序"，之后所有绘制都由它渲染。
        //   为什么每帧调用：当前程序是全局状态。场景里有多种材质(多个程序)时，
        //         画不同物体前要来回切换。本项目只有一个程序，重复调用无害。
        glUseProgram(shaderProgram);                     // 启用我们的着色器

        // 用时间驱动一个 [-0.2, 0.2] 的正弦偏移，三角形就会左右摆动
        //
        // ◆ glfwGetTime()
        //   功能：返回 glfwInit() 至今经过的秒数（double 类型）。
        //   为什么用它而不是自己累计帧数：帧率会波动，用"真实时间"算出的动画
        //         在任何机器、任何帧率下速度都一致——这叫"时间驱动"而非"帧驱动"，
        //         是做动画的标准思路。
        float t = (float)glfwGetTime();

        // sinf 是 C 标准库的正弦函数：输入弧度，输出 [-1,1] 的周期波动值，
        // 乘 0.2 后得到 [-0.2,0.2] 的来回偏移（x 坐标加上它，三角形就左右摆）。
        // t*2.0 里的 2.0 是摆动速度，数字越大摆得越快。
        //
        // ◆ glGetUniformLocation(程序ID, uniform变量名)
        //   功能：查询着色器里 uniform 变量 "xOffset" 在程序中的位置（可理解为编号）。
        //   为什么需要它：CPU 无法直接摸到 GPU 内部变量，只能先拿到"位置号"，
        //         再用 glUniformXxx 往这个位置写值。若变量名写错或没被使用，
        //         返回 -1（赋值会被静默忽略，不报错——查不到值时先怀疑拼写）。
        //   为什么写在循环里：位置号其实不变，挪到循环外只查一次更高效；
        //         教程为了"查位置->赋值"两行连着便于理解，就放这儿了。
        //
        // ◆ glUniform1f(位置号, 值)
        //   功能：给指定位置写入 1 个 float（1f = 1个float；还有 glUniform3f/
        //         glUniformMatrix4fv 等，按数据类型选函数）。
        //   为什么这么用：这是 CPU -> GPU 传"每帧都可能变的参数"的标准通道
        //         （动画、变换矩阵都走 uniform）。
        //   前提：必须先 glUseProgram，否则赋值给的不是"当前程序"，无效。
        glUniform1f(glGetUniformLocation(shaderProgram, "xOffset"), 0.2f * sinf(t * 2.0f));

        // ◆ glBindVertexArray(VAO)
        //   功能：把配置好的 VAO 绑为"当前 VAO"，GPU 画图时按它记录的说明书读顶点。
        //   为什么画图前必须绑：glDrawArrays 只认"当前 VAO"。多物体的项目里，
        //         画每个物体前各绑各的 VAO，顶点数据就自动切换了。
        glBindVertexArray(VAO);

        // ◆ glDrawArrays(图元类型, 起始顶点索引, 用几个顶点)
        //   功能：正式开画！从当前 VAO 读顶点 -> 顶点着色器逐个处理 -> 组装成图元
        //         -> 光栅化成像素 -> 片段着色器逐像素上色。
        //   参数：GL_TRIANGLES 表示每 3 个顶点拼 1 个三角形；(0,3) = 用第 0~2 个
        //         共 3 个顶点，正好一个三角形。想画两个三角形：准备 6 个顶点，
        //         最后一个参数传 6。
        //   整个文件最核心的一句——前面几十行初始化，全是为它做准备。
        glDrawArrays(GL_TRIANGLES, 0, 3);                // 画！3 个顶点组成一个三角形

        // ◆ glfwSwapBuffers(窗口句柄)
        //   功能：交换窗口的"前缓冲"和"后缓冲"。
        //   原理（双缓冲）：整帧画面先偷偷画在后缓冲（屏幕看不见的地方），全部画完
        //         再一次性换到前缓冲显示。为什么这么设计：如果边画边显示，屏幕会
        //         看到"只画了一半的三角形"，画面闪烁撕裂。配合 glfwSwapInterval(1)，
        //         交换会等到屏幕刷新间隙（垂直同步）才执行。
        glfwSwapBuffers(window);    // 前后缓冲交换，把画好的内容显示出来

        // ◆ glfwPollEvents()
        //   功能：把操作系统攒下的事件队列处理一遍：键盘、鼠标、窗口移动/缩放、
        //         点关闭按钮……该触发回调的触发回调（如 framebuffer_size_callback），
        //         该设标志的设标志（如点了 X 就设"应该关闭"）。
        //   为什么每帧必须调：不调用的话事件堆着没人理——点 X 没反应、拖窗口
        //         不重绘，程序看起来像"死"了。
        glfwPollEvents();           // 检查有没有键盘/窗口事件
    }

    // ========================= 5. 清理资源 =========================
    // 惯例：先删 OpenGL 对象，最后 glfwTerminate 收尾。窗口程序退出时系统会
    // 兜底回收大部分资源，所以不写这几行往往也"没事"，但规范清理是好习惯——
    // 长期运行、多窗口的程序不清理，显存会越占越多。

    // ◆ glDeleteVertexArrays(个数, ID数组) / glDeleteBuffers(个数, ID数组)
    //   功能：删除 VAO / VBO，释放显存和驱动资源。
    //   为什么显式删：OpenGL 对象没有 C++ 析构函数那套自动回收，程序不退出
    //         就一直占着资源。与 glGenXxx 一一对应，申请了就要还。
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // ◆ glDeleteProgram(程序ID)
    //   功能：删除着色器程序，释放它占用的资源。与 glCreateProgram 对应。
    glDeleteProgram(shaderProgram);

    // ◆ glfwTerminate()
    //   功能：关闭 GLFW、销毁窗口和上下文，释放全部 GLFW 资源。
    //   为什么放在最后：它会连带销毁所有窗口，必须等窗口和 OpenGL 对象都用完
    //         再调。与 glfwInit() 成对出现，程序走到这就 return 了。
    glfwTerminate();
    return 0;
}
