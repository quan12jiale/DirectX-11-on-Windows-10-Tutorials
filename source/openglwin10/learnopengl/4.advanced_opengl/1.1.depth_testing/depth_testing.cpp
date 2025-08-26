#include "learnopengl/4.advanced_opengl/1.1.depth_testing/depth_testing.h"
#include "learnopengl/shader.h"
#include "util.h"

DepthTesting::DepthTesting()
    : camera(QVector3D(0.0f, 0.0f, 3.0f))
{
}

bool DepthTesting::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    bool result;

    // Create and initialize the OpenGL object.
    m_OpenGL = new OpenGLClass;

    result = m_OpenGL->Initialize2(hwnd, screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH, VSYNC_ENABLED);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize OpenGL", L"Error", MB_OK);
        return false;
    }

    // configure global opengl state
    // -----------------------------
    m_OpenGL->glEnable(GL_DEPTH_TEST);
    m_OpenGL->glDepthFunc(GL_ALWAYS); // always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))

    // build and compile our shader zprogram
    // ------------------------------------
    ourShader = new Shader("1.1.depth_testing.vs", "1.1.depth_testing.fs", m_OpenGL);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cubeVertices[] = {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    float planeVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };

    // cube VAO
    //unsigned int cubeVAO, cubeVBO;
    m_OpenGL->glGenVertexArrays(1, &cubeVAO);
    m_OpenGL->glGenBuffers(1, &cubeVBO);
    m_OpenGL->glBindVertexArray(cubeVAO);
    m_OpenGL->glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    m_OpenGL->glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    // position attribute
    m_OpenGL->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    m_OpenGL->glEnableVertexAttribArray(0);
    // texture coord attribute
    m_OpenGL->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    m_OpenGL->glEnableVertexAttribArray(1);
    m_OpenGL->glBindVertexArray(0);

    // plane VAO
    //unsigned int planeVAO, planeVBO;
    m_OpenGL->glGenVertexArrays(1, &planeVAO);
    m_OpenGL->glGenBuffers(1, &planeVBO);
    m_OpenGL->glBindVertexArray(planeVAO);
    m_OpenGL->glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    m_OpenGL->glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    m_OpenGL->glEnableVertexAttribArray(0);
    m_OpenGL->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    m_OpenGL->glEnableVertexAttribArray(1);
    m_OpenGL->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    m_OpenGL->glBindVertexArray(0);

    // load textures
    // -------------
    unsigned int cubeTexture = Util::loadTexture("resources/textures/marble.jpg", m_OpenGL);
    unsigned int floorTexture = Util::loadTexture("resources/textures/metal.png", m_OpenGL);

    // shader configuration
    // --------------------
    ourShader->use(); // don't forget to activate/use the shader before setting uniforms!
    ourShader->setInt("texture1", 0);

    elapsed_timer_.start();
    return true;
}

void DepthTesting::Shutdown()
{
    if (m_OpenGL)
    {
        m_OpenGL->glDeleteVertexArrays(1, &cubeVAO);
        m_OpenGL->glDeleteVertexArrays(1, &planeVAO);
        m_OpenGL->glDeleteBuffers(1, &cubeVBO);
        m_OpenGL->glDeleteBuffers(1, &planeVBO);

        m_OpenGL->Shutdown();
        delete m_OpenGL;
        m_OpenGL = 0;
    }
    if (ourShader) {
        delete ourShader;
        ourShader = nullptr;
    }
}

bool DepthTesting::Frame()
{
    // per-frame time logic
    // --------------------
    float currentFrame = elapsed_timer_.elapsed();
    deltaTime = (currentFrame - lastFrame) / 1000.0f;
    lastFrame = currentFrame;

    // input
    // -----
    processInput();

    m_OpenGL->MakeCurrent();
    // render
    // ------
    m_OpenGL->glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    m_OpenGL->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

    // activate shader
    ourShader->use();

    QMatrix4x4 model;
    QMatrix4x4 view = camera.GetViewMatrix();
    QMatrix4x4 projection;
    projection.perspective(camera.Zoom, (float)800 / (float)600, 0.1f, 100.0f);
    ourShader->setMat4("view", view);
    ourShader->setMat4("projection", projection);

    // cubes
    m_OpenGL->glBindVertexArray(cubeVAO);
    m_OpenGL->glActiveTexture(GL_TEXTURE0);
    m_OpenGL->glBindTexture(GL_TEXTURE_2D, cubeTexture);
    model.translate(QVector3D(-1.0f, 0.0f, -1.0f));
    ourShader->setMat4("model", model);
    m_OpenGL->glDrawArrays(GL_TRIANGLES, 0, 36);
    model.setToIdentity();
    model.translate(QVector3D(2.0f, 0.0f, 0.0f));
    ourShader->setMat4("model", model);
    m_OpenGL->glDrawArrays(GL_TRIANGLES, 0, 36);

    // floor
    m_OpenGL->glBindVertexArray(planeVAO);
    m_OpenGL->glBindTexture(GL_TEXTURE_2D, floorTexture);
    ourShader->setMat4("model", QMatrix4x4());
    m_OpenGL->glDrawArrays(GL_TRIANGLES, 0, 6);
    m_OpenGL->glBindVertexArray(0);

    // swap buffers
    m_OpenGL->EndScene();
    return true;
}

void DepthTesting::SetMsg(const MSG& msg)
{
    m_msg = msg;
}

void DepthTesting::processInput()
{
    if (m_msg.message == WM_KEYDOWN) {
        switch (m_msg.wParam) {
        case 'W':
            camera.ProcessKeyboard(FORWARD, deltaTime);
            break;
        case 'S':
            camera.ProcessKeyboard(BACKWARD, deltaTime);
            break;
        case 'A':
            camera.ProcessKeyboard(LEFT, deltaTime);
            break;
        case 'D':
            camera.ProcessKeyboard(RIGHT, deltaTime);
            break;
        default:
            break;
        }
    }
    mouse_callback();
    scroll_callback();
}

void DepthTesting::mouse_callback()
{
    // 检查消息类型是否为鼠标移动消息
    if (m_msg.message == WM_MOUSEMOVE)
    {
        float xpos = static_cast<float>(LOWORD(m_msg.lParam));
        float ypos = static_cast<float>(HIWORD(m_msg.lParam));

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void DepthTesting::scroll_callback()
{
    // 检查消息类型是否为鼠标滚轮消息
    if (m_msg.message == WM_MOUSEWHEEL)
    {
        // 获取滚轮的偏移量
        int zDelta = GET_WHEEL_DELTA_WPARAM(m_msg.wParam);
        double yoffset = static_cast<double>(zDelta) / WHEEL_DELTA; // 转换为标准单位

        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}
