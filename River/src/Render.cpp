#include "Render.h"
#include <GL/glew.h>
#include <iostream>

FBO::FBO(unsigned int width, unsigned int height)
{
    glGenFramebuffers(1, &ID);
    glBindFramebuffer(GL_FRAMEBUFFER, ID);

    // generate FBO color attachment, bind to current FBO
    glGenTextures(1, &ColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ColorBuffer);  // bind texture

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);        // unbind texture

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorBuffer, 0);

    // depth
    glGenTextures(1, &DepthBuffer);
    glBindTexture(GL_TEXTURE_2D, DepthBuffer);  // bind texture

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindTexture(GL_TEXTURE_2D, 0);        // unbind texture

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthBuffer, 0);


    // check if framebuffer created successfullly
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Render::RenderWaveParticle(WaveParticleMesh& waveParticleMesh, int pointSize, FBO& fbo)
{
	glPointSize(pointSize);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo.ID);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	waveParticleShader.Bind();

	glBindVertexArray(waveParticleMesh.VAO);
	glDrawArrays(GL_POINTS, 0, waveParticleMesh.size);

	waveParticleShader.unBind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}