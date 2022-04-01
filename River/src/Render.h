#pragma once
#include "Mesh.h"
#include "Shader.h"


class FBO
{
public:
	FBO(unsigned int width, unsigned int height);

	void AddTarget(unsigned int width, unsigned int height);

	unsigned int ID, DepthBuffer, ColorBuffer1, ColorBuffer2;
};


struct Setting
{
	int blurSize = 50;
	int particleSize = 1;
	float dx = 1;
	float dz = 1;

	int tessellationFactor = 37;
	bool enableWireframeMode = false;
	float heightFactor = 1.5f;
};



class Render
{
public:
	// wave particle visialization, only dots
    Shader waveParticleShader{ "res/Shaders/WaveParticle.vert", "res/Shaders/WaveParticle.frag"};

	// horizontal blur, used to generate height map base on wave particles
	// technically, it is blurring.
	// from the project perspective, it's one of two passes of height map reconstruction base on wave equations
	Shader horozontalBlur{ "res/Shaders/Blur.vert", "res/Shaders/HorizontalBlur.frag" };

	// vertical blur, used to generate height map base on wave particles
	// the second pass of wave reconstruction
	Shader verticalBlur{ "res/Shaders/Blur.vert", "res/Shaders/VerticalBlur.frag" };


	Shader quadShader{ "res/Shaders/Blur.vert", "res/Shaders/Quad.frag" };

	Shader waveMeshShader{  "res/Shaders/wave.vert",
							"res/Shaders/wave.tesc",
							"res/Shaders/wave.tese",
							"res/Shaders/wave.frag" };

	unsigned int quadVAO, quadVBO;

public:
	Render();
	~Render();

	void RenderWaveParticle(WaveParticleMesh& waveParticleMesh, unsigned int fbo);

	void HorizontalBlur(unsigned int inputTexture, unsigned int fbo);
	void VerticalBlur(unsigned int f123, unsigned int f45v, unsigned int fbo);

	void RenderWaveMesh(unsigned int deviation, unsigned int gradient, unsigned int fbo);

	void DrawQuad(unsigned int inputTexture);
};