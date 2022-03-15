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
    Shader waveParticleShader{ "res/Shaders/WaveParticle.vert", "res/Shaders/WaveParticle.frag"};



	void RenderWaveParticle(WaveParticleMesh& waveParticleMesh, int pointSize, FBO &fbo);


};