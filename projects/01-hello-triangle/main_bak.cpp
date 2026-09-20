// ============================================================================
// 01-hello-triangle —— 你的第一个 OpenGL 程序
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
// ============================================================================

#include <glad/glad.h>   // 注意：glad.h 必须写在 glfw3.h 之前
#include <GLFW/glfw3.h>  // GLFW 负责创建窗口、处理键盘鼠标输入
#include <cstdio>
#include <cmath>

// ---------- 回调：窗口大小改变时，把 OpenGL 视口同步成新尺寸 ----------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// ---------- 处理输入：目前只处理 ESC 退出 ----------
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // ==================== 1. 初始化 GLFW，创建窗口 ====================
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // 告诉 GLFW 我们要
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);  // OpenGL 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 核心模式(现代可编程管线)

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL: Hello Triangle", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);      // 把这个窗口设为当前上下文
    glfwSwapInterval(1);                 // 开启垂直同步，防止占满 CPU/GPU
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ================== 2. 用 GLAD 加载 OpenGL 函数指针 ==================
    // OpenGL 的函数地址是运行时从显卡驱动里查出来的，不加载就调用会直接崩溃
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    // 打印当前环境信息：看看你的显卡驱动提供了哪个版本的 OpenGL
    printf("OpenGL version : %s\n", (const char*)glGetString(GL_VERSION));
    printf("GPU renderer   : %s\n", (const char*)glGetString(GL_RENDERER));
    printf("Press ESC to quit.\n\n");
    fflush(stdout);  // 立刻把上面几行显示到控制台

    glViewport(0, 0, 800, 600);  // 告诉 OpenGL 窗口哪块区域用来渲染

    // ============ 3. 着色器 + 顶点数据（可编程管线的核心概念） ============
    // ---- 3.1 顶点着色器：GPU 上运行，负责处理每一个顶点 ----
    // 输入 3 个顶点位置，输出到裁剪空间；xOffset 是每帧传入的偏移量(动画)
    const char* vertexShaderSource = R"(#version 330 core
layout (location = 0) in vec3 aPos;     // 位置属性，location=0
layout (location = 1) in vec3 aColor;   // 颜色属性，location=1
uniform float xOffset;                  // uniform: 每帧由 CPU 传进来的全局变量
out vec3 ourColor;                      // 传给片段着色器的插值颜色
void main() {
    gl_Position = vec4(aPos.x + xOffset, aPos.y, aPos.z, 1.0);
    ourColor = aColor;
})";

    // ---- 3.2 片段着色器：决定屏幕上每个像素最终是什么颜色 ----
    const char* fragmentShaderSource = R"(#version 330 core
in vec3 ourColor;    // 从顶点着色器插值而来
out vec4 FragColor;  // 输出的像素颜色
void main() {
    FragColor = vec4(ourColor, 1.0);
})";

    // 编译两个着色器（出错时打印编译日志，这是最常用的调试手段）
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success; char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) { glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR vertex shader:\n%s\n", infoLog); return -1; }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) { glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR fragment shader:\n%s\n", infoLog); return -1; }

    // 链接成着色器程序(shader program)，之后渲染时就要"使用"它
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) { glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR link:\n%s\n", infoLog); return -1; }
    glDeleteShader(vertexShader);     // 链接完成后，中间产物可以删掉
    glDeleteShader(fragmentShader);

    // ---- 3.3 三角形的顶点数据：每行 = (x,y,z) 位置 + (r,g,b) 颜色 ----
    float vertices[] = {
        //   位置              颜色
        -0.5f, -0.5f, 0.0f,  1.0f, 0.5f, 0.2f,   // 左下：橙
         0.5f, -0.5f, 0.0f,  0.2f, 0.8f, 0.3f,   // 右下：绿
         0.0f,  0.6f, 0.0f,  0.2f, 0.5f, 1.0f,   // 顶上：蓝
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);   // VAO 记住"顶点数据长什么样、怎么读"
    glGenBuffers(1, &VBO);        // VBO 是真正存放顶点数据的显存块
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 告诉 OpenGL 怎么解析这段内存：
    // 属性0=位置，3 个 float，从第 0 个字节开始，每 6 个 float(24字节) 一组
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 属性1=颜色，3 个 float，每组内偏移 3 个 float(12字节) 处开始
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // ========================= 4. 渲染循环 =========================
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.1f, 0.12f, 0.15f, 1.0f);         // 设置清屏色(深蓝灰)
        glClear(GL_COLOR_BUFFER_BIT);                    // 用清屏色填满屏幕

        glUseProgram(shaderProgram);                     // 启用我们的着色器
        // 用时间驱动一个 [-0.2, 0.2] 的正弦偏移，三角形就会左右摆动
        float t = (float)glfwGetTime();
        glUniform1f(glGetUniformLocation(shaderProgram, "xOffset"), 0.2f * sinf(t * 2.0f));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);                // 画！3 个顶点组成一个三角形

        glfwSwapBuffers(window);    // 前后缓冲交换，把画好的内容显示出来
        glfwPollEvents();           // 检查有没有键盘/窗口事件
    }

    // ========================= 5. 清理资源 =========================
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
