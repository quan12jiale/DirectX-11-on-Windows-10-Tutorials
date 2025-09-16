#ifndef SHADER_H
#define SHADER_H

#include <QMatrix4x4>
#include <QOpenGLExtraFunctions>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    QOpenGLExtraFunctions* m_OpenGL;
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath, QOpenGLExtraFunctions* openGL)
        : m_OpenGL(openGL)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = m_OpenGL->glCreateShader(GL_VERTEX_SHADER);
        m_OpenGL->glShaderSource(vertex, 1, &vShaderCode, NULL);
        m_OpenGL->glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = m_OpenGL->glCreateShader(GL_FRAGMENT_SHADER);
        m_OpenGL->glShaderSource(fragment, 1, &fShaderCode, NULL);
        m_OpenGL->glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = m_OpenGL->glCreateProgram();
        m_OpenGL->glAttachShader(ID, vertex);
        m_OpenGL->glAttachShader(ID, fragment);
        m_OpenGL->glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        m_OpenGL->glDeleteShader(vertex);
        m_OpenGL->glDeleteShader(fragment);
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use() 
    { 
        m_OpenGL->glUseProgram(ID);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {         
        m_OpenGL->glUniform1i(m_OpenGL->glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    { 
        m_OpenGL->glUniform1i(m_OpenGL->glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    { 
        m_OpenGL->glUniform1f(m_OpenGL->glGetUniformLocation(ID, name.c_str()), value);
    }
    void setVec2(const std::string &name, float x, float y) const
    { 
        m_OpenGL->glUniform2f(m_OpenGL->glGetUniformLocation(ID, name.c_str()), x, y);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    { 
        m_OpenGL->glUniform3f(m_OpenGL->glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) 
    { 
        m_OpenGL->glUniform4f(m_OpenGL->glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, const QMatrix4x4& mat) const
    {
        m_OpenGL->glUniformMatrix4fv(m_OpenGL->glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, mat.constData());
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if(type != "PROGRAM")
        {
            m_OpenGL->glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                m_OpenGL->glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            m_OpenGL->glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                m_OpenGL->glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
#endif
