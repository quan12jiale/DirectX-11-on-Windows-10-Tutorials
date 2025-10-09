////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "lightshaderclass.h"
#include "util.h"

using namespace std;

LightShaderClass::LightShaderClass()
{
    m_OpenGLPtr = 0;
}


LightShaderClass::LightShaderClass(const LightShaderClass& other)
{
}


LightShaderClass::~LightShaderClass()
{
}


bool LightShaderClass::Initialize(OpenGLClass* OpenGL)
{
    char vsFilename[128];
    char psFilename[128];
    bool result;


    // Store the pointer to the OpenGL object.
    m_OpenGLPtr = OpenGL;

    // Set the location and names of the shader files.
    strcpy(vsFilename, "../OpenglEngine/light.vs");
    strcpy(psFilename, "../OpenglEngine/light.ps");

    // Initialize the vertex and pixel shaders.
    result = InitializeShader(vsFilename, psFilename);
    if(!result)
    {
        return false;
    }

    return true;
}


void LightShaderClass::Shutdown()
{
    // Shutdown the shader.
    ShutdownShader();

    // Release the pointer to the OpenGL object.
    m_OpenGLPtr = 0;

    return;
}


bool LightShaderClass::InitializeShader(char* vsFilename, char* fsFilename)
{
    const char* vertexShaderBuffer;
    const char* fragmentShaderBuffer;
    int status;


    // Load the vertex shader source file into a text buffer.
    vertexShaderBuffer = LoadShaderSourceFile(vsFilename);
    if(!vertexShaderBuffer)
    {
        return false;
    }

    // Load the fragment shader source file into a text buffer.
    fragmentShaderBuffer = LoadShaderSourceFile(fsFilename);
    if(!fragmentShaderBuffer)
    {
        return false;
    }

    // Create a shader program object.
    m_shaderProgram = m_OpenGLPtr->glCreateProgram();

    // Create a vertex and fragment shader object.
    m_vertexShader = Util::compileShader(m_shaderProgram, GL_VERTEX_SHADER, vertexShaderBuffer, m_OpenGLPtr);
    m_fragmentShader = Util::compileShader(m_shaderProgram, GL_FRAGMENT_SHADER, fragmentShaderBuffer, m_OpenGLPtr);

    // Release the vertex and fragment shader buffers.
    delete[] vertexShaderBuffer;
    vertexShaderBuffer = 0;

    delete[] fragmentShaderBuffer;
    fragmentShaderBuffer = 0;

    // Bind the shader input variables.
    m_OpenGLPtr->glBindAttribLocation(m_shaderProgram, 0, "inputPosition");
    m_OpenGLPtr->glBindAttribLocation(m_shaderProgram, 1, "inputTexCoord");
    m_OpenGLPtr->glBindAttribLocation(m_shaderProgram, 2, "inputNormal");

    // Link the shader program.
    m_OpenGLPtr->glLinkProgram(m_shaderProgram);
    if (!Util::checkLinkProgramError(m_shaderProgram, m_OpenGLPtr)) {
        return false;
    }

    return true;
}


void LightShaderClass::ShutdownShader()
{
    // Detach the vertex and fragment shaders from the program.
    m_OpenGLPtr->glDetachShader(m_shaderProgram, m_vertexShader);
    m_OpenGLPtr->glDetachShader(m_shaderProgram, m_fragmentShader);

    // Delete the vertex and fragment shaders.
    m_OpenGLPtr->glDeleteShader(m_vertexShader);
    m_OpenGLPtr->glDeleteShader(m_fragmentShader);

    // Delete the shader program.
    m_OpenGLPtr->glDeleteProgram(m_shaderProgram);

    return;
}


char* LightShaderClass::LoadShaderSourceFile(char* filename)
{
    FILE* filePtr;
    char* buffer;
    long fileSize, count;
    int error;


    // Open the shader file for reading in text modee.
    filePtr = fopen(filename, "r");
    if(filePtr == NULL)
    {
        return 0;
    }

    // Go to the end of the file and get the size of the file.
    fseek(filePtr, 0, SEEK_END);
    fileSize = ftell(filePtr);

    // Initialize the buffer to read the shader source file into, adding 1 for an extra null terminator.
    buffer = new char[fileSize + 1];

    // Return the file pointer back to the beginning of the file.
    fseek(filePtr, 0, SEEK_SET);

    // Read the shader text file into the buffer.
    count = fread(buffer, 1, fileSize, filePtr);
    if(count != fileSize)
    {
        return 0;
    }

    // Close the file.
    error = fclose(filePtr);
    if(error != 0)
    {
        return 0;
    }

    // Null terminate the buffer.
    buffer[fileSize] = '\0';

    return buffer;
}


bool LightShaderClass::SetShaderParameters(float* worldMatrix, float* viewMatrix, float* projectionMatrix, float* lightDirection, float* diffuseLightColor)
{
    float tpWorldMatrix[16], tpViewMatrix[16], tpProjectionMatrix[16];
    int location;


    // Transpose the matrices to prepare them for the shader.
    m_OpenGLPtr->MatrixTranspose(tpWorldMatrix, worldMatrix);
    m_OpenGLPtr->MatrixTranspose(tpViewMatrix, viewMatrix);
    m_OpenGLPtr->MatrixTranspose(tpProjectionMatrix, projectionMatrix);
    
    // Install the shader program as part of the current rendering state.
    m_OpenGLPtr->glUseProgram(m_shaderProgram);

    // Set the world matrix in the vertex shader.
    location = m_OpenGLPtr->glGetUniformLocation(m_shaderProgram, "worldMatrix");
    if(location == -1)
    {
        cout << "World matrix not set." << endl;
    }
    m_OpenGLPtr ->glUniformMatrix4fv(location, 1, false, tpWorldMatrix);

    // Set the view matrix in the vertex shader.
    location = m_OpenGLPtr->glGetUniformLocation(m_shaderProgram, "viewMatrix");
    if(location == -1)
    {
        cout << "View matrix not set." << endl;
    }
    m_OpenGLPtr->glUniformMatrix4fv(location, 1, false, tpViewMatrix);

    // Set the projection matrix in the vertex shader.
    location = m_OpenGLPtr->glGetUniformLocation(m_shaderProgram, "projectionMatrix");
    if(location == -1)
    {
        cout << "Projection matrix not set." << endl;
    }
    m_OpenGLPtr->glUniformMatrix4fv(location, 1, false, tpProjectionMatrix);

    // Set the texture in the pixel shader to use the data from the first texture unit.
    location = m_OpenGLPtr->glGetUniformLocation(m_shaderProgram, "shaderTexture");
    if(location == -1)
    {
        cout << "Shader texture not set." << endl;
    }
    m_OpenGLPtr->glUniform1i(location, 0);

    // Set the light direction in the pixel shader.
    location = m_OpenGLPtr->glGetUniformLocation(m_shaderProgram, "lightDirection");
    if(location == -1)
    {
        cout << "Light direction not set." << endl;
    }
    m_OpenGLPtr->glUniform3fv(location, 1, lightDirection);

    // Set the diffuse light color in the pixel shader.
    location = m_OpenGLPtr->glGetUniformLocation(m_shaderProgram, "diffuseLightColor");
    if(location == -1)
    {
        cout << "Diffuse light color not set." << endl;
    }
    m_OpenGLPtr->glUniform4fv(location, 1, diffuseLightColor);

    return true;
}
