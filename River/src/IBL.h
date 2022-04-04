#pragma once

#include "Shader.h"
#include <stb_image.h>
#include <string>


const unsigned int CaptureResolution = 1024;

unsigned int LoadHDR(std::string path);

void renderCube();


class Skybox
{
public:
	Skybox();

	void GenerateCubemap(unsigned int textureID);

	void Render();

private:
	unsigned int envCubemap;
	unsigned int captureFBO;
	unsigned int captureRBO;

	Shader generateCubemapShader{ "res/Shaders/GenerateCubemap.vert", "res/Shaders/GenerateCubemap.frag" };
	Shader SkyboxShader{ "res/Shaders/Skybox.vert", "res/Shaders/Skybox.frag" };

	void InitTextures();
	void InitFBO();
};