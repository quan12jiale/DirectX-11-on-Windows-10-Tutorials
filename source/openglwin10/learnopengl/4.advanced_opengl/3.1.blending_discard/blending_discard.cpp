#include "learnopengl/4.advanced_opengl/3.1.blending_discard/blending_discard.h"
#include "learnopengl/shader.h"
#include "util.h"
#include <QKeyEvent>

BlendingDiscard::BlendingDiscard()
	: camera(QVector3D(0.0f, 0.0f, 3.0f))
{
}

BlendingDiscard::~BlendingDiscard()
{
}

void BlendingDiscard::initializeGL()
{
	this->resize(800, 600);
	this->setWindowTitle("LearnOpenGL");
	this->makeCurrent();
	this->initializeOpenGLFunctions();

	this->glEnable(GL_DEPTH_TEST);

	ourShader = new Shader("3.1.blending.vs", "3.1.blending.fs", this);

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
    float transparentVertices[] = {
        // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };
    // cube VAO
    //unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    // plane VAO
    //unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    // transparent VAO
    //unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    // load textures
    // -------------
    cubeTexture = Util::loadTexture("resources/textures/marble.jpg",this);
    floorTexture = Util::loadTexture("resources/textures/metal.png", this);
    transparentTexture = Util::loadTexture("resources/textures/grass.png", this);

    // transparent vegetation locations
// --------------------------------
    vegetation = 
    {
        QVector3D(-1.5f, 0.0f, -0.48f),
        QVector3D(1.5f, 0.0f, 0.51f),
        QVector3D(0.0f, 0.0f, 0.7f),
        QVector3D(-0.3f, 0.0f, -2.3f),
        QVector3D(0.5f, 0.0f, -0.6f)
    };

    // shader configuration
    // --------------------
    ourShader->use(); // don't forget to activate/use the shader before setting uniforms!
    ourShader->setInt("texture1", 0);

    elapsed_timer_.start();
}

void BlendingDiscard::resizeGL(int w, int h)
{
    this->glViewport(0, 0, w, h);
    this->update();
}

void BlendingDiscard::paintGL()
{
    // per-frame time logic
        // --------------------
    float currentFrame = elapsed_timer_.elapsed();
    deltaTime = (currentFrame - lastFrame) / 1000.0f;
    lastFrame = currentFrame;

    // input
    // -----
    //processInput(window);

    // render
    // ------
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw objects
    ourShader->use();
    QMatrix4x4 model;
    QMatrix4x4 view = camera.GetViewMatrix();
    QMatrix4x4 projection;
    projection.perspective(camera.Zoom, (float)800 / (float)600, 0.1f, 100.0f);
    ourShader->setMat4("projection", projection);
    ourShader->setMat4("view", view);
    // cubes
    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    model.translate(QVector3D(-1.0f, 0.0f, -1.0f));
    ourShader->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model.setToIdentity();
    model.translate(QVector3D(2.0f, 0.0f, 0.0f));
    ourShader->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // floor
    glBindVertexArray(planeVAO);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    model.setToIdentity();
    ourShader->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // vegetation
    glBindVertexArray(transparentVAO);
    glBindTexture(GL_TEXTURE_2D, transparentTexture);
    for (unsigned int i = 0; i < vegetation.size(); i++)
    {
        model.setToIdentity();
        model.translate(vegetation[i]);
        ourShader->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }


    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    //glfwSwapBuffers(window);
    //glfwPollEvents();
}

void BlendingDiscard::keyPressEvent(QKeyEvent* ev)
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

void BlendingDiscard::mouseMoveEvent(QMouseEvent* ev)
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

void BlendingDiscard::wheelEvent(QWheelEvent* ev)
{
    const QPoint angleDelta = ev->angleDelta();
    // 获取滚轮的偏移量
    double yoffset = static_cast<double>(angleDelta.y()) / 120.0; // 120 是标准单位

    camera.ProcessMouseScroll(static_cast<float>(yoffset));

    ev->accept();
    this->update();
}
