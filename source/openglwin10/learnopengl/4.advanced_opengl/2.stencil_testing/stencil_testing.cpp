#include "learnopengl/4.advanced_opengl/2.stencil_testing/stencil_testing.h"
#include "learnopengl/shader.h"
#include "util.h"
#include <QKeyEvent>

StencilTesting::StencilTesting()
	: camera(QVector3D(0.0f, 0.0f, 3.0f))
{
}

StencilTesting::~StencilTesting()
{
    this->glDeleteVertexArrays(1, &cubeVAO);
    this->glDeleteVertexArrays(1, &planeVAO);
    this->glDeleteBuffers(1, &cubeVBO);
    this->glDeleteBuffers(1, &planeVBO);

    if (ourShader) {
        delete ourShader;
        ourShader = nullptr;
    }
    if (ourShaderSingleColor) {
        delete ourShaderSingleColor;
        ourShaderSingleColor = nullptr;
    }
}

void StencilTesting::initializeGL()
{
	this->resize(800, 600);
    this->setTitle("LearnOpenGL");
	initializeOpenGLFunctions();
	this->makeCurrent();

    // configure global opengl state
    // -----------------------------
    this->glEnable(GL_DEPTH_TEST);
    this->glDepthFunc(GL_LESS);
    this->glEnable(GL_STENCIL_TEST);
    this->glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    this->glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // build and compile our shader zprogram
    // ------------------------------------
    ourShader = new Shader("2.stencil_testing.vs", "2.stencil_testing.fs", this);
    ourShaderSingleColor = new Shader("2.stencil_testing.vs", "2.stencil_single_color.fs", this);

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
    this->glGenVertexArrays(1, &cubeVAO);
    this->glGenBuffers(1, &cubeVBO);
    this->glBindVertexArray(cubeVAO);
    this->glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    this->glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    // position attribute
    this->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    this->glEnableVertexAttribArray(0);
    // texture coord attribute
    this->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    this->glEnableVertexAttribArray(1);
    this->glBindVertexArray(0);

    // plane VAO
    //unsigned int planeVAO, planeVBO;
    this->glGenVertexArrays(1, &planeVAO);
    this->glGenBuffers(1, &planeVBO);
    this->glBindVertexArray(planeVAO);
    this->glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    this->glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    this->glEnableVertexAttribArray(0);
    this->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    this->glEnableVertexAttribArray(1);
    this->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    this->glBindVertexArray(0);

    // load textures
    // -------------
    cubeTexture = Util::loadTexture("resources/textures/marble.jpg", this);
    floorTexture = Util::loadTexture("resources/textures/metal.png", this);

    // shader configuration
    // --------------------
    ourShader->use(); // don't forget to activate/use the shader before setting uniforms!
    ourShader->setInt("texture1", 0);

    elapsed_timer_.start();
}

void StencilTesting::resizeGL(int w, int h)
{
    this->glViewport(0, 0, w, h);
    this->update(); // 请求重绘
}

void StencilTesting::paintGL()
{
    // per-frame time logic
    // --------------------
    float currentFrame = elapsed_timer_.elapsed();
    deltaTime = (currentFrame - lastFrame) / 1000.0f;
    lastFrame = currentFrame;

    // input
    // -----
    //processInput();

    this->makeCurrent();
    // render
    // ------
    this->glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // don't forget to clear the stencil buffer!

    // activate shader
    ourShaderSingleColor->use();

    QMatrix4x4 model;
    QMatrix4x4 view = camera.GetViewMatrix();
    QMatrix4x4 projection;
    projection.perspective(camera.Zoom, (float)800 / (float)600, 0.1f, 100.0f);
    ourShaderSingleColor->setMat4("view", view);
    ourShaderSingleColor->setMat4("projection", projection);

    ourShader->use();
    ourShader->setMat4("view", view);
    ourShader->setMat4("projection", projection);

    // draw floor as normal, but don't write the floor to the stencil buffer, we only care about the containers. We set its mask to 0x00 to not write to the stencil buffer.
    glStencilMask(0x00);
    // floor
    glBindVertexArray(planeVAO);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    ourShader->setMat4("model", QMatrix4x4());
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // 1st. render pass, draw objects as normal, writing to the stencil buffer
    // --------------------------------------------------------------------
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    // cubes
    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    model.translate(QVector3D(-1.0f, 0.0f, -1.0f));
    ourShader->setMat4("model", model);
    this->glDrawArrays(GL_TRIANGLES, 0, 36);
    model.setToIdentity();
    model.translate(QVector3D(2.0f, 0.0f, 0.0f));
    ourShader->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // floor
    this->glBindVertexArray(planeVAO);
    this->glBindTexture(GL_TEXTURE_2D, floorTexture);
    ourShader->setMat4("model", QMatrix4x4());
    this->glDrawArrays(GL_TRIANGLES, 0, 6);
    this->glBindVertexArray(0);

    // 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
    // Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
    // the objects' size differences, making it look like borders.
    // -----------------------------------------------------------------------------------------------------------------------------
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    ourShaderSingleColor->use();
    float scale = 1.1f;
    // cubes
    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    model.setToIdentity();
    model.translate(QVector3D(-1.0f, 0.0f, -1.0f));
    model.scale(QVector3D(scale, scale, scale));
    ourShaderSingleColor->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model.setToIdentity();
    model.translate(QVector3D(2.0f, 0.0f, 0.0f));
    model.scale(QVector3D(scale, scale, scale));
    ourShaderSingleColor->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glEnable(GL_DEPTH_TEST);

    // swap buffers
    //this->EndScene();
}

void StencilTesting::keyPressEvent(QKeyEvent* ev)
{
    switch (ev->key()) {
    case Qt::Key_W:
        camera.ProcessKeyboard(FORWARD, deltaTime);
        break;
    case Qt::Key_S:
        camera.ProcessKeyboard(BACKWARD, deltaTime);
        break;
    case Qt::Key_A:
        camera.ProcessKeyboard(LEFT, deltaTime);
        break;
    case Qt::Key_D:
        camera.ProcessKeyboard(RIGHT, deltaTime);
        break;
    default:
        break;
    }
    this->update(); // 请求重绘
}

void StencilTesting::mouseMoveEvent(QMouseEvent* ev)
{
    if (ev->buttons() == Qt::NoButton) {
        return;
    }
    const QPointF localPos = ev->localPos();
    float xpos = localPos.x();
    float ypos = localPos.y();

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
    this->update(); // 请求重绘
}

void StencilTesting::wheelEvent(QWheelEvent* ev)
{
    const QPoint angleDelta = ev->angleDelta();
    // 获取滚轮的偏移量
    double yoffset = static_cast<double>(angleDelta.y()) / 120.0; // 120 是标准单位

    camera.ProcessMouseScroll(static_cast<float>(yoffset));

    ev->accept();
    this->update(); // 请求重绘
}


