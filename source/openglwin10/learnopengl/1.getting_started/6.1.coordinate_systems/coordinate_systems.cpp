#include "learnopengl/1.getting_started/6.1.coordinate_systems/coordinate_systems.h"
#include "learnopengl/shader.h"

bool CoordinateSystems::Initialize(int screenWidth, int screenHeight, HWND hwnd)
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


	// build and compile our shader zprogram
	// ------------------------------------
	ourShader = new Shader("6.1.coordinate_systems.vs", "6.1.coordinate_systems.fs", m_OpenGL);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    //unsigned int VBO, VAO, EBO;
    m_OpenGL->glGenVertexArrays(1, &VAO);
    m_OpenGL->glGenBuffers(1, &VBO);
    m_OpenGL->glGenBuffers(1, &EBO);

    m_OpenGL->glBindVertexArray(VAO);

    m_OpenGL->glBindBuffer(GL_ARRAY_BUFFER, VBO);
    m_OpenGL->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    m_OpenGL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    m_OpenGL->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    m_OpenGL->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    m_OpenGL->glEnableVertexAttribArray(0);
    // texture coord attribute
    m_OpenGL->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    m_OpenGL->glEnableVertexAttribArray(1);

    // load and create a texture 
    // -------------------------
    //unsigned int texture1, texture2;
    // texture 1
    // ---------
    m_OpenGL->glGenTextures(1, &texture1);
    m_OpenGL->glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    m_OpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    m_OpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    m_OpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    m_OpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    //unsigned char* data = stbi_load(FileSystem::getPath("resources/textures/container.jpg").c_str(), &width, &height, &nrChannels, 0);
    QImage image("resources/textures/container.jpg");
    if (!image.isNull()) {
        image.mirror();//stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
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
    //data = stbi_load(FileSystem::getPath("resources/textures/awesomeface.png").c_str(), &width, &height, &nrChannels, 0);
    QImage image2("resources/textures/awesomeface.png");
    if (!image2.isNull()) {
        image2.mirror();
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
    // or set it via the texture class
    ourShader->setInt("texture1", 0);
    ourShader->setInt("texture2", 1);
    return true;
}

void CoordinateSystems::Shutdown()
{
    if (m_OpenGL)
    {
        m_OpenGL->glDeleteVertexArrays(1, &VAO);
        m_OpenGL->glDeleteBuffers(1, &VBO);
        m_OpenGL->glDeleteBuffers(1, &EBO);

        m_OpenGL->Shutdown();
        delete m_OpenGL;
        m_OpenGL = 0;
    }
    if (ourShader) {
        delete ourShader;
        ourShader = nullptr;
    }
}

bool CoordinateSystems::Frame()
{
    m_OpenGL->MakeCurrent();
    // render
    // ------
    m_OpenGL->glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    m_OpenGL->glClear(GL_COLOR_BUFFER_BIT);

    // bind textures on corresponding texture units
    m_OpenGL->glActiveTexture(GL_TEXTURE0);
    m_OpenGL->glBindTexture(GL_TEXTURE_2D, texture1);
    m_OpenGL->glActiveTexture(GL_TEXTURE1);
    m_OpenGL->glBindTexture(GL_TEXTURE_2D, texture2);

    // activate shader
    ourShader->use();

    // create transformations
    QMatrix4x4 model;
    QMatrix4x4 view;
    QMatrix4x4 projection;
    model.rotate(-55.0f, QVector3D(1.0f, 0.0f, 0.0f));
    view.translate(QVector3D(0.0f, 0.0f, -3.0f));
    projection.perspective(45.0f, (float)800 / (float)600, 0.1f, 100.0f);

    // retrieve the matrix uniform locations
    unsigned int modelLoc = m_OpenGL->glGetUniformLocation(ourShader->ID, "model");
    unsigned int viewLoc = m_OpenGL->glGetUniformLocation(ourShader->ID, "view");
    // pass them to the shaders (3 different ways)
    m_OpenGL->glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.constData());
    m_OpenGL->glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.constData());
    // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
    ourShader->setMat4("projection", projection);

    // render container
    m_OpenGL->glBindVertexArray(VAO);
    m_OpenGL->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // swap buffers
    m_OpenGL->EndScene();
    return true;
}
