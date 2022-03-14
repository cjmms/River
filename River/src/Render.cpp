#include "Render.h"
#include <GL/glew.h>


void Render::RenderWaveParticle(WaveParticleMesh& waveParticleMesh)
{
	glPointSize(5);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	waveParticleShader.Bind();

	glBindVertexArray(waveParticleMesh.VAO);
	glDrawArrays(GL_POINTS, 0, waveParticleMesh.size);

	waveParticleShader.unBind();
}