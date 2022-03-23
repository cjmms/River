#pragma once
#include "Mesh.h"
#include "Shader.h"


class FBO
{
public:
	FBO(unsigned int width, unsigned int height);
	unsigned int ID, DepthBuffer, ColorBuffer;
};




class Render
{
public:
	// wave particle visialization, only dots
    Shader waveParticleShader{ "res/Shaders/WaveParticle.vert", "res/Shaders/WaveParticle.frag"};

	// horizontal blur, used to generate height map base on wave particles
	// technical perspective, it is blurring.
	// from the project perspective, it's one of two passes og height map generation base on wave equations
	Shader horozontalBlur{ "res/Shaders/Blur.vert", "res/Shaders/HorizontalBlur.frag" };

	unsigned int quadVAO, quadVBO;

public:
	Render();
	~Render();

	void RenderWaveParticle(WaveParticleMesh& waveParticleMesh, int pointSize, unsigned int fbo);

	void HorizontalBlur(unsigned int inputTexture, unsigned int fbo);
};