#include "Shader.h"
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>




Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    // 1. read and parse shaders
    std::string vertexCode;
    std::string fragmentCode;

    ReadShaderFile(vertexPath, vertexCode);
    ReadShaderFile(fragmentPath, fragmentCode);

    // 2. compile shaders
    unsigned int vertex, fragment;

    CompileShader(vertex, vertexCode, GL_VERTEX_SHADER);
    CompileShader(fragment, fragmentCode, GL_FRAGMENT_SHADER);

    // 3. link shaders
    int success;
    char infoLog[512];

    // shader Program
    rendererID = glCreateProgram();
    glAttachShader(rendererID, vertex);
    glAttachShader(rendererID, fragment);
    glLinkProgram(rendererID);
    // print linking errors if any
    glGetProgramiv(rendererID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(rendererID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}




Shader::Shader(const std::string& vertexPath,
    const std::string& tcsPath,
    const std::string& tesPath,
    const std::string& fragmentPath)
{
    // 1. read and parse shaders
    std::string vertexCode;
    std::string tcsCode;
    std::string tesCode;
    std::string fragmentCode;

    ReadShaderFile(vertexPath, vertexCode);
    ReadShaderFile(tcsPath, tcsCode);
    ReadShaderFile(tesPath, tesCode);
    ReadShaderFile(fragmentPath, fragmentCode);

    // 2. compile shaders
    unsigned int vertex, tcs, tes, fragment;

    CompileShader(vertex, vertexCode, GL_VERTEX_SHADER);
    CompileShader(tcs, tcsCode, GL_TESS_CONTROL_SHADER);
    CompileShader(tes, tesCode, GL_TESS_EVALUATION_SHADER);
    CompileShader(fragment, fragmentCode, GL_FRAGMENT_SHADER);

    // 3. link shaders
    int success;
    char infoLog[512];

    // shader Program
    rendererID = glCreateProgram();
    glAttachShader(rendererID, vertex);
    glAttachShader(rendererID, tcs);
    glAttachShader(rendererID, tes);
    glAttachShader(rendererID, fragment);
    glLinkProgram(rendererID);
    // print linking errors if any
    glGetProgramiv(rendererID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(rendererID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(tcs);
    glDeleteShader(tes);
    glDeleteShader(fragment);
}





Shader::~Shader()
{
    glDeleteProgram(rendererID);
}



void Shader::ReadShaderFile(const std::string& filePath, std::string& code)
{
    std::ifstream shaderFile;

    // ensure ifstream objects can throw exceptions:
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        shaderFile.open(filePath);

        std::stringstream shaderStream;
        // read file's buffer contents into streams
        shaderStream << shaderFile.rdbuf();

        shaderFile.close();

        // convert stream into string
        code = shaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        std::cout << "File name: " << filePath << std::endl;
    }
}



void Shader::CompileShader(unsigned int& shaderUnit, const std::string& shaderCode, int shaderType)
{
    int success;
    char infoLog[512];

    const char* ShaderCode = shaderCode.c_str();

    // vertex Shader
    shaderUnit = glCreateShader(shaderType);
    glShaderSource(shaderUnit, 1, &ShaderCode, NULL);
    glCompileShader(shaderUnit);
    // print compile errors if any
    glGetShaderiv(shaderUnit, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderUnit, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    };
}



void Shader::Bind()
{
    glUseProgram(rendererID);
}

void Shader::unBind()
{
    glUseProgram(0);
}


int Shader::getUniformLocation(const char* name)
{
    return glGetUniformLocation(rendererID, name);
}

unsigned int Shader::getRendererID()
{
    return rendererID;
}


void Shader::setMat4(const char* name, glm::mat4 matrix)
{
    this->Bind();
    unsigned int location = getUniformLocation(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    this->unBind();
}


void Shader::setMat4(const std::string& name, glm::mat4 matrix)
{
    setMat4(name.c_str(), matrix);
}



void Shader::setVec3(const char* name, glm::vec3 vec)
{
    this->Bind();
    unsigned int location = getUniformLocation(name);
    glUniform3fv(location, 1, glm::value_ptr(vec));
    this->unBind();
}


void Shader::setVec3(const std::string& name, glm::vec3 vec)
{
    setVec3(name.c_str(), vec);
}



void Shader::setVec2(const char* name, glm::vec2 vec)
{
    unsigned int location = getUniformLocation(name);
    glUniform2fv(location, 1, glm::value_ptr(vec));
}


void Shader::setVec2(const std::string& name, glm::vec2 vec)
{
    setVec2(name.c_str(), vec);
}



void Shader::setFloat(const char* name, float value)
{
    this->Bind();
    unsigned int location = getUniformLocation(name);
    glUniform1f(location, value);
    this->unBind();
}



void Shader::setFloat(const std::string& name, float value)
{
    setFloat(name.c_str(), value);
}



void Shader::setInt(const char* name, int value)
{
    this->Bind();
    unsigned int location = getUniformLocation(name);
    glUniform1i(location, value);
    this->unBind();
}



void Shader::setInt(const std::string& name, int value)
{
    setInt(name.c_str(), value);
}


void Shader::setTexture(const char* name, unsigned int texture, int Type)
{

    auto it = std::find(textures.begin(), textures.end(), name);

    // If element was found
    if (it != textures.end())
    {
        // calculating the index
        int index = it - textures.begin();

        this->setInt(name, index);

        this->Bind();
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(Type, texture);
        this->unBind();
    }
    else {
        // If the element is not
        // present in the vector

        this->setInt(name, textureUnit);

        this->Bind();
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(Type, texture);
        this->unBind();

        textureUnit++;
        textures.push_back(name);
    }
}


void Shader::setTexture(const std::string& name, unsigned int texture, int index, int Type)
{
    setTexture(name.c_str(), texture, index, Type);
}
