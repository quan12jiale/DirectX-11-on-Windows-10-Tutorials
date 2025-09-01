#include "learnopengl/1.getting_started/7.1.camera_circle/camera_circle.h"
#include "learnopengl/shader.h"

bool CameraCircle::Initialize(int screenWidth, int screenHeight, HWND hwnd)
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

    // build and compile our shader zprogram
    // ------------------------------------
    ourShader = new Shader("7.1.camera.vs", "7.1.camera.fs", m_OpenGL);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
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
    // world space positions of our cubes
    cubePositions = {
        QVector3D(0.0f,  0.0f,  0.0f),
        QVector3D(2.0f,  5.0f, -15.0f),
        QVector3D(-1.5f, -2.2f, -2.5f),
        QVector3D(-3.8f, -2.0f, -12.3f),
        QVector3D(2.4f, -0.4f, -3.5f),
        QVector3D(-1.7f,  3.0f, -7.5f),
        QVector3D(1.3f, -2.0f, -2.5f),
        QVector3D(1.5f,  2.0f, -2.5f),
        QVector3D(1.5f,  0.2f, -1.5f),
        QVector3D(-1.3f,  1.0f, -1.5f)
    };

    const unsigned int instanceCount = 10;

    // calculate the model matrix for all instances
    QGenericMatrix<4, 4, float> modelMatrices[instanceCount];
    for (unsigned int i = 0; i < instanceCount; i++)
    {
        QMatrix4x4 model;
        model.translate(cubePositions[i]);
        float angle = 20.0f * i;
        model.rotate(angle, QVector3D(1.0f, 0.3f, 0.5f));
        modelMatrices[i] = model.toGenericMatrix<4, 4>();
    }

    //unsigned int VBO, VAO;
    m_OpenGL->glGenVertexArrays(1, &VAO);
    m_OpenGL->glGenBuffers(1, &VBO);

    m_OpenGL->glBindVertexArray(VAO);

    m_OpenGL->glBindBuffer(GL_ARRAY_BUFFER, VBO);
    m_OpenGL->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    m_OpenGL->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    m_OpenGL->glEnableVertexAttribArray(0);
    // texture coord attribute
    m_OpenGL->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    m_OpenGL->glEnableVertexAttribArray(1);

    // Create Instance Buffer (Instance Buffer)
    const std::size_t mat4Size = sizeof(QGenericMatrix<4, 4, float>);
    //unsigned int instanceVBO;
    m_OpenGL->glGenBuffers(1, &instanceVBO);
    m_OpenGL->glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    m_OpenGL->glBufferData(GL_ARRAY_BUFFER, mat4Size * instanceCount, modelMatrices[0].constData(), GL_STATIC_DRAW);

    // Set matrix properties (mat4 requires 4 vec4)
    const std::size_t vec4Size = sizeof(float) * 4;
    m_OpenGL->glEnableVertexAttribArray(2);
    m_OpenGL->glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    m_OpenGL->glEnableVertexAttribArray(3);
    m_OpenGL->glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    m_OpenGL->glEnableVertexAttribArray(4);
    m_OpenGL->glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    m_OpenGL->glEnableVertexAttribArray(5);
    m_OpenGL->glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    // Set instantiation properties
    m_OpenGL->glVertexAttribDivisor(2, 1);
    m_OpenGL->glVertexAttribDivisor(3, 1);
    m_OpenGL->glVertexAttribDivisor(4, 1);
    m_OpenGL->glVertexAttribDivisor(5, 1);

    // load and create a texture 
    // -------------------------
    //unsigned int texture1, texture2;
    // texture 1
    // ---------
    m_OpenGL->glGenTextures(1, &texture1);
    m_OpenGL->glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    m_OpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    m_OpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    m_OpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    m_OpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    QImage image("resources/textures/container.jpg");
    if (!image.isNull()) {
        image = image.mirrored();//stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        // QImage::Format_RGB32:The image is stored using a 32-bit RGB format (0xffRRGGBB).
        if (image.format() != QImage::Format_RGB888) {
            image = image.convertToFormat(QImage::Format_RGB888);
        }
        m_OpenGL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.constBits());
        m_OpenGL->glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    // texture 2
    // ---------
    m_OpenGL->glGenTextures(1, &texture2);
    m_OpenGL->glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    m_OpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    m_OpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    m_OpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    m_OpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    QImage image2("resources/textures/awesomeface.png");
    if (!image2.isNull()) {
        image2 = image2.mirrored();
        // QImage::Format_RGB32:The image is stored using a 32-bit RGB format (0xffRRGGBB).
        if (image2.format() != QImage::Format_RGBA8888) {
            image2 = image2.convertToFormat(QImage::Format_RGBA8888);
        }
        m_OpenGL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image2.width(), image2.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image2.constBits());
        m_OpenGL->glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader->use(); // don't forget to activate/use the shader before setting uniforms!
    ourShader->setInt("texture1", 0);
    ourShader->setInt("texture2", 1);

    // pass projection matrix to shader (as projection matrix rarely changes there's no need to do this per frame)
    // -----------------------------------------------------------------------------------------------------------
    QMatrix4x4 projection;
    projection.perspective(45.0f, (float)800 / (float)600, 0.1f, 100.0f);
    ourShader->setMat4("projection", projection);

    elapsed_timer_.start();
    return true;
}

void CameraCircle::Shutdown()
{
    if (m_OpenGL)
    {
        m_OpenGL->glDeleteVertexArrays(1, &VAO);
        m_OpenGL->glDeleteBuffers(1, &VBO);
        m_OpenGL->glDeleteBuffers(1, &instanceVBO);

        m_OpenGL->Shutdown();
        delete m_OpenGL;
        m_OpenGL = 0;
    }
    if (ourShader) {
        delete ourShader;
        ourShader = nullptr;
    }
}

bool CameraCircle::Frame()
{
    m_OpenGL->MakeCurrent();
    // render
    // ------
    m_OpenGL->glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    m_OpenGL->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

    // bind textures on corresponding texture units
    m_OpenGL->glActiveTexture(GL_TEXTURE0);
    m_OpenGL->glBindTexture(GL_TEXTURE_2D, texture1);
    m_OpenGL->glActiveTexture(GL_TEXTURE1);
    m_OpenGL->glBindTexture(GL_TEXTURE_2D, texture2);

    // activate shader
    ourShader->use();

    // camera/view transformation
    // Rotate 1 radians in 1 second
    float angle = elapsed_timer_.elapsed() / 1000.0f;
    QMatrix4x4 view;
    float radius = 10.0f;
    float camX = static_cast<float>(sin(angle) * radius);
    float camZ = static_cast<float>(cos(angle) * radius);
    view.lookAt(QVector3D(camX, 0.0f, camZ), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
    ourShader->setMat4("view", view);

    // instances rendering - render all cubes with one drawing call
    m_OpenGL->glBindVertexArray(VAO);
    m_OpenGL->glDrawArraysInstanced(GL_TRIANGLES, 0, 36, cubePositions.size());

    // swap buffers
    m_OpenGL->EndScene();
    return true;
}
