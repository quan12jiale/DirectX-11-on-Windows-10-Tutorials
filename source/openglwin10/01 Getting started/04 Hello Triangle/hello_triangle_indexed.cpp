#include "01 Getting started/04 Hello Triangle/hello_triangle_indexed.h"
#include "util.h"

static const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
static const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

bool HelloTriangleIndexed::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// Create and initialize the OpenGL object.
	m_OpenGL = new OpenGLClass;

	result = m_OpenGL->Initialize(hwnd, screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH, VSYNC_ENABLED);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize OpenGL", L"Error", MB_OK);
		return false;
	}

	// build and compile our shader program
	// ------------------------------------
	// link shaders
	shaderProgram = m_OpenGL->glCreateProgram();

	// vertex shader
	unsigned int vertexShader = Util::compileShader(shaderProgram, GL_VERTEX_SHADER, vertexShaderSource, m_OpenGL);
	// fragment shader
	unsigned int fragmentShader = Util::compileShader(shaderProgram, GL_FRAGMENT_SHADER, fragmentShaderSource, m_OpenGL);

	m_OpenGL->glLinkProgram(shaderProgram);
	if (!Util::checkLinkProgramError(shaderProgram, m_OpenGL)) {
		return false;
	}

	m_OpenGL->glDeleteShader(vertexShader);
	m_OpenGL->glDeleteShader(fragmentShader);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	m_OpenGL->glGenVertexArrays(1, &VAO);
	m_OpenGL->glGenBuffers(1, &VBO);
	m_OpenGL->glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	m_OpenGL->glBindVertexArray(VAO);

	m_OpenGL->glBindBuffer(GL_ARRAY_BUFFER, VBO);
	m_OpenGL->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	m_OpenGL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	m_OpenGL->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	/*
	glVertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr)
	QRhiVertexInputBinding(quint32 stride, Classification cls = PerVertex, quint32 stepRate = 1)
	QRhiVertexInputAttribute(int binding, int location, Format format, quint32 offset, int matrixSlice = -1)

	GLuint indx：顶点着色器中使用 layout (location = 0) 定义了position顶点属性的位置值(Location)，QRhiVertexInputAttribute中的int location
	GLint size：QRhiVertexInputAttribute中的Format format，Float4的话size就是4，Float3的话size就是3
	GLenum type：QRhiVertexInputAttribute中的Format format，Float4的话type就是GL_FLOAT，UInt3的话type就是GL_UNSIGNED_INT
	GLboolean normalized：QRhiVertexInputAttribute中的Format format，UNormByte4的话normalized就是true
	GLsizei stride：QRhiVertexInputBinding中的quint32 stride
	const void* ptr：QRhiVertexInputAttribute中的reinterpret_cast<const GLvoid *>(quintptr(quint32 offset))
	*/
	m_OpenGL->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	/*
	glEnableVertexAttribArray(GLuint index)
	GLuint indx：顶点着色器中使用 layout (location = 0) 定义了position顶点属性的位置值(Location)，QRhiVertexInputAttribute中的int location
	*/
	m_OpenGL->glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	m_OpenGL->glBindBuffer(GL_ARRAY_BUFFER, 0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	//m_OpenGL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	m_OpenGL->glBindVertexArray(0);

	auto glPolygonMode = reinterpret_cast<void(QOPENGLF_APIENTRYP)(GLenum, GLenum)>(
		m_OpenGL->GetGLContext()->getProcAddress(QByteArrayLiteral("glPolygonMode")));
	if (glPolygonMode) {
		// uncomment this call to draw in wireframe polygons.
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	m_OpenGL->TurnZBufferOff();
	return true;
}

void HelloTriangleIndexed::Shutdown()
{
	// Release the OpenGL object.
	if (m_OpenGL)
	{
		m_OpenGL->glDeleteVertexArrays(1, &VAO);
		m_OpenGL->glDeleteBuffers(1, &VBO);
		m_OpenGL->glDeleteBuffers(1, &EBO);
		m_OpenGL->glDeleteProgram(shaderProgram);

		m_OpenGL->Shutdown();
		delete m_OpenGL;
		m_OpenGL = 0;
	}
}

bool HelloTriangleIndexed::Frame()
{
	// render
	m_OpenGL->MakeCurrent();
	m_OpenGL->glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	GLboolean depthTestEnabled;
	m_OpenGL->glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
	if (depthTestEnabled == GL_TRUE) {
		// Note：如果启用了深度测试，在每次渲染之前，需要清除深度缓冲。否则中间的正方形不会显示
		m_OpenGL->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else {
		m_OpenGL->glClear(GL_COLOR_BUFFER_BIT);
	}

	// draw our first triangle
	m_OpenGL->glUseProgram(shaderProgram);
	m_OpenGL->glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	m_OpenGL->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// swap buffers
	m_OpenGL->EndScene();
	return true;
}

