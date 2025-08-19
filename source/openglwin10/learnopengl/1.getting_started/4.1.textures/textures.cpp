#include "learnopengl/1.getting_started/4.1.textures/textures.h"
#include "learnopengl/shader.h"

bool Textures::Initialize(int screenWidth, int screenHeight, HWND hwnd)
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
    ourShader = new Shader("4.1.texture.vs", "4.1.texture.fs", m_OpenGL);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    m_OpenGL->glGenVertexArrays(1, &VAO);
    m_OpenGL->glGenBuffers(1, &VBO);
    m_OpenGL->glGenBuffers(1, &EBO);

    m_OpenGL->glBindVertexArray(VAO);

    m_OpenGL->glBindBuffer(GL_ARRAY_BUFFER, VBO);
    m_OpenGL->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    m_OpenGL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    m_OpenGL->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    m_OpenGL->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    m_OpenGL->glEnableVertexAttribArray(0);
    // color attribute
    m_OpenGL->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    m_OpenGL->glEnableVertexAttribArray(1);
    // texture coord attribute
    m_OpenGL->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    m_OpenGL->glEnableVertexAttribArray(2);

    // load and create a texture 
    // -------------------------
    //unsigned int texture;
    m_OpenGL->glGenTextures(1, &texture);
    m_OpenGL->glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    m_OpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    m_OpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    m_OpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    m_OpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    QImage image("resources/textures/container.jpg");
    if (!image.isNull()) {
        // QImage::Format_RGB32:The image is stored using a 32-bit RGB format (0xffRRGGBB).
        if (image.format() != QImage::Format_RGB888) {
            image = image.convertToFormat(QImage::Format_RGB888);
        }
        m_OpenGL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.constBits());
        m_OpenGL->glGenerateMipmap(GL_TEXTURE_2D);
    }

	return true;
}

void Textures::Shutdown()
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

bool Textures::Frame()
{
    m_OpenGL->MakeCurrent();
    // render
    // ------
    m_OpenGL->glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    m_OpenGL->glClear(GL_COLOR_BUFFER_BIT);

    // bind Texture
    m_OpenGL->glBindTexture(GL_TEXTURE_2D, texture);

    // render container
    ourShader->use();
    m_OpenGL->glBindVertexArray(VAO);
    m_OpenGL->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // swap buffers
    m_OpenGL->EndScene();
	return true;
}
