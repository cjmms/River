#pragma once
#include "Mesh.h"
#include "Shader.h"

class Render
{
public:
    Shader waveParticleShader{ "res/Shaders/WaveParticle.vert", "res/Shaders/WaveParticle.frag"};



	void RenderWaveParticle(WaveParticleMesh& waveParticleMesh);


};