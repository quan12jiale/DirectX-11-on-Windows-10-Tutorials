#include "learnopengl/4.advanced_opengl/5.2.framebuffers_exercise1/framebuffers_exercise1.h"
#include "learnopengl/shader.h"
#include "util.h"
#include <QKeyEvent>

FrameBuffersExercise1::FrameBuffersExercise1()
    : camera(QVector3D(0.0f, 0.0f, 3.0f))
{
}

FrameBuffersExercise1::~FrameBuffersExercise1()
{
    this->glDeleteVertexArrays(1, &cubeVAO);
    this->glDeleteVertexArrays(1, &planeVAO);
    this->glDeleteVertexArrays(1, &quadVAO);
    this->glDeleteBuffers(1, &cubeVBO);
    this->glDeleteBuffers(1, &planeVBO);
    this->glDeleteBuffers(1, &quadVBO);
    this->glDeleteRenderbuffers(1, &rbo);
    this->glDeleteFramebuffers(1, &framebuffer);
    
    // 清理纹理资源
    this->glDeleteTextures(1, &cubeTexture);
    this->glDeleteTextures(1, &floorTexture);
    this->glDeleteTextures(1, &textureColorbuffer);

    if (shader) {
        delete shader;
        shader = nullptr;
    }
    if (screenShader) {
        delete screenShader;
        screenShader = nullptr;
    }
}

void FrameBuffersExercise1::initializeGL()
{
    this->resize(800, 600);
    this->setWindowTitle("LearnOpenGL");
    this->makeCurrent();
    this->initializeOpenGLFunctions();

    // configure global opengl state
    // -----------------------------
    this->glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    shader = new Shader("5.2.framebuffers.vs", "5.2.framebuffers.fs", this);
    screenShader = new Shader("5.2.framebuffers_screen.vs", "5.2.framebuffers_screen.fs", this);

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
        // positions          // texture Coords 
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };
    // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates. NOTE that this plane is now much smaller and at the top of the screen
    float quadVertices[] = {
        // positions   // texCoords
        -0.3f,  1.0f,  0.0f, 1.0f,
        -0.3f,  0.7f,  0.0f, 0.0f,
         0.3f,  0.7f,  1.0f, 0.0f,

        -0.3f,  1.0f,  0.0f, 1.0f,
         0.3f,  0.7f,  1.0f, 0.0f,
         0.3f,  1.0f,  1.0f, 1.0f
    };
    // cube VAO
    //unsigned int cubeVAO, cubeVBO;
    this->glGenVertexArrays(1, &cubeVAO);
    this->glGenBuffers(1, &cubeVBO);
    this->glBindVertexArray(cubeVAO);
    this->glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    this->glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    this->glEnableVertexAttribArray(0);
    this->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    this->glEnableVertexAttribArray(1);
    this->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
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
    // screen quad VAO
    //unsigned int quadVAO, quadVBO;
    this->glGenVertexArrays(1, &quadVAO);
    this->glGenBuffers(1, &quadVBO);
    this->glBindVertexArray(quadVAO);
    this->glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    this->glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    this->glEnableVertexAttribArray(0);
    this->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    this->glEnableVertexAttribArray(1);
    this->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // load textures
    // -------------
    cubeTexture = Util::loadTexture("resources/textures/container.jpg", this);
    floorTexture = Util::loadTexture("resources/textures/metal.png", this);

    // shader configuration
    // --------------------
    shader->use();
    shader->setInt("texture1", 0);

    screenShader->use();
    screenShader->setInt("screenTexture", 0);

    // framebuffer configuration
    // -------------------------
    //unsigned int framebuffer;
    this->glGenFramebuffers(1, &framebuffer);
    this->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    //unsigned int textureColorbuffer;
    this->glGenTextures(1, &textureColorbuffer);
    this->glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    this->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width(), this->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    this->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    this->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    this->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    //unsigned int rbo;
    this->glGenRenderbuffers(1, &rbo);
    this->glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    this->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->width(), this->height()); // use a single renderbuffer object for both a depth AND stencil buffer.
    this->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (this->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    this->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    elapsed_timer_.start();
}

void FrameBuffersExercise1::resizeGL(int w, int h)
{
    this->glViewport(0, 0, w, h);
    this->update();
}

void FrameBuffersExercise1::paintGL()
{
    // per-frame time logic
    // --------------------
    float currentFrame = elapsed_timer_.elapsed();
    deltaTime = (currentFrame - lastFrame) / 1000.0f;
    lastFrame = currentFrame;

    // input
    // -----
    //processInput();

    // bind to framebuffer and draw scene as we normally would to color texture 
    this->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    this->glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

    // make sure we clear the framebuffer's content
    this->glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->use();
    QMatrix4x4 model;
    camera.Yaw += 180.0f; // rotate the camera's yaw 180 degrees around
    camera.ProcessMouseMovement(0, 0, false); // call this to make sure it updates its camera vectors, note that we disable pitch constrains for this specific case (otherwise we can't reverse camera's pitch values)
    QMatrix4x4 view = camera.GetViewMatrix();
    camera.Yaw -= 180.0f; // reset it back to its original orientation
    camera.ProcessMouseMovement(0, 0, true);
    QMatrix4x4 projection;
    projection.perspective(camera.Zoom, (float)this->width() / (float)this->height(), 0.1f, 100.0f);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    // cubes
    this->glBindVertexArray(cubeVAO);
    this->glActiveTexture(GL_TEXTURE0);
    this->glBindTexture(GL_TEXTURE_2D, cubeTexture);
    model.translate(QVector3D(-1.0f, 0.0f, -1.0f));
    shader->setMat4("model", model);
    this->glDrawArrays(GL_TRIANGLES, 0, 36);
    model.setToIdentity();
    model.translate(QVector3D(2.0f, 0.0f, 0.0f));
    shader->setMat4("model", model);
    this->glDrawArrays(GL_TRIANGLES, 0, 36);
    // floor
    this->glBindVertexArray(planeVAO);
    this->glBindTexture(GL_TEXTURE_2D, floorTexture);
    shader->setMat4("model", QMatrix4x4());
    this->glDrawArrays(GL_TRIANGLES, 0, 6);
    this->glBindVertexArray(0);

    // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
    this->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    model.setToIdentity();
    view = camera.GetViewMatrix();
    shader->setMat4("view", view);

    // cubes
    this->glBindVertexArray(cubeVAO);
    this->glActiveTexture(GL_TEXTURE0);
    this->glBindTexture(GL_TEXTURE_2D, cubeTexture);
    model.translate(QVector3D(-1.0f, 0.0f, -1.0f));
    shader->setMat4("model", model);
    this->glDrawArrays(GL_TRIANGLES, 0, 36);
    model.setToIdentity();
    model.translate(QVector3D(2.0f, 0.0f, 0.0f));
    shader->setMat4("model", model);
    this->glDrawArrays(GL_TRIANGLES, 0, 36);
    // floor
    this->glBindVertexArray(planeVAO);
    this->glBindTexture(GL_TEXTURE_2D, floorTexture);
    shader->setMat4("model", QMatrix4x4());
    this->glDrawArrays(GL_TRIANGLES, 0, 6);
    this->glBindVertexArray(0);

    // now draw the mirror quad with screen texture
    // --------------------------------------------
    this->glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

    screenShader->use();
    this->glBindVertexArray(quadVAO);
    this->glBindTexture(GL_TEXTURE_2D, textureColorbuffer); // use the color attachment texture as the texture of the quad plane
    this->glDrawArrays(GL_TRIANGLES, 0, 6);
}

void FrameBuffersExercise1::keyPressEvent(QKeyEvent* ev)
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
    this->update();
}

void FrameBuffersExercise1::mouseMoveEvent(QMouseEvent* ev)
{
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
    this->update();
}

void FrameBuffersExercise1::wheelEvent(QWheelEvent* ev)
{
    const QPoint angleDelta = ev->angleDelta();
    // 获取滚轮的偏移量
    double yoffset = static_cast<double>(angleDelta.y()) / 120.0; // 120 是标准单位

    camera.ProcessMouseScroll(static_cast<float>(yoffset));

    ev->accept();
    this->update();
}
