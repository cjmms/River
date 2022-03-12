#pragma once
#include <string>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>



class Shader
{
private:
	// program ID of shader
	unsigned int rendererID;
	unsigned int textureUnit;
	std::vector<std::string> textures;

public:
	Shader() :rendererID(0) {};
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	~Shader();
	void Bind();
	void unBind();
	int getUniformLocation(const char* name);
	unsigned int getRendererID();

	void setMat4(const char* name, glm::mat4 matrix);
	void setMat4(const std::string& name, glm::mat4 matrix);
	void setVec3(const char* name, glm::vec3 vec);
	void setVec3(const std::string& name, glm::vec3 vec);
	void setVec2(const char* name, glm::vec2 vec);
	void setVec2(const std::string& name, glm::vec2 vec);
	void setFloat(const char* name, float value);
	void setFloat(const std::string& name, float value);

	void setInt(const char* name, int value);
	void setInt(const std::string& name, int value);

	void setTexture(const char* name, unsigned int texture);
	void setTexture(const std::string& name, unsigned int texture, int index = -1);

private:
	// path: res/Shaders/basic.shader
	// path is relative a path

	// 1. retrieve the vertex/fragment source code from filePath
	void ReadShaderFile(const std::string& filePath, std::string& code);

	// 2. compile shaders
	void CompileShader(unsigned int& shaderUnit, const std::string& shaderCode, int shaderType);
};