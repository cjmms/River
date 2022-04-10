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

class PingPong
{
public:
	PingPong(FBO* first, FBO* second) : ping{first}, pong{second}
	{}
	~PingPong()
	{
		if (ping) delete ping;
		if (pong) delete pong;
	}
	FBO* ping = nullptr;
	FBO* pong = nullptr;

	void Swap()
	{
		FBO* temp = ping;
		pong = ping;
		ping = temp;
	}
};

struct Setting
{
	int blurSize = 50;
	int particleSize = 1;
	float dx = 0.01;
	float dz = 0.01;

	int tessellationFactor = 37;
	bool enableWireframeMode = false;
	float heightFactor = 0.15f;

	int seletectedRenderPass = 5;

	// lighting
	float waterDepth = 9.3f;
	float extinctionCoeff  = 0.242f;
	glm::vec3 waterBedColor = glm::vec3(0.4, 0.3, 0.2);
	float FoamTurbulance = 0.043;

	float timeScale = 2;
	bool enableNormalMap = false;

	int obstacleParticleSize = 3;
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

	Shader createObstacleShader{ "res/Shaders/CreateObstacle.vert", "res/Shaders/CreateObstacle.frag" };

	Shader flowAdvect			{"res/Shaders/FlowMap/simple.vert", "res/Shaders/FlowMap/advect.frag"};
	Shader flowComputeDivergence{"res/Shaders/FlowMap/simple.vert", "res/Shaders/FlowMap/computeDivergence.frag"};
	Shader flowJacobi			{"res/Shaders/FlowMap/simple.vert", "res/Shaders/FlowMap/jacobi.frag"};
	Shader flowSubtractGradient {"res/Shaders/FlowMap/simple.vert", "res/Shaders/FlowMap/subtractGradient.frag"};

	unsigned int quadVAO, quadVBO, quadPatchVAO;

public:
	Render();
	~Render();

	void InitFlowMapBindings(const FBO& fboVeloctiyPressure, const FBO& fboObstacle, const FBO& fboDivergence )
	{
		// Pre-emptive bindings for the flow shaders:
		flowAdvect.Bind();
		glUniform1i(0, fboObstacle.ColorBuffer1);
		glUniform1i(1, fboVeloctiyPressure.ColorBuffer1); // vel

		flowComputeDivergence.Bind();
		glUniform1i(0, fboObstacle.ColorBuffer1);
		glUniform1i(1, fboVeloctiyPressure.ColorBuffer1); // vel

		flowJacobi.Bind();
		glUniform1i(0, fboObstacle.ColorBuffer1);
		glUniform1i(2, fboVeloctiyPressure.ColorBuffer2); // pressure
		glUniform1i(3, fboDivergence.ColorBuffer1);

		flowSubtractGradient.Bind();
		glUniform1i(0, fboObstacle.ColorBuffer1);
		glUniform1i(1, fboVeloctiyPressure.ColorBuffer1); // vel
		glUniform1i(2, fboVeloctiyPressure.ColorBuffer2); // pressure
	}

	void UpdateFlowMap();

	void RenderWaveParticle(WaveParticleMesh& waveParticleMesh, unsigned int fbo);

	void HorizontalBlur(unsigned int inputTexture, unsigned int fbo);
	void VerticalBlur(unsigned int f123, unsigned int f45v, unsigned int fbo);

	void RenderWaveMesh(unsigned int irradianceMap, unsigned int skybox, unsigned int deviation, unsigned int gradient, unsigned int fbo);

	void RenderObstacles(unsigned int fbo);

	void DrawQuad(unsigned int inputTexture);

	void DebugDraw( unsigned int particleMap, 
					unsigned int f123, 
					unsigned int f45v, 
					unsigned int deviation,
					unsigned int gradient,
					unsigned int waveMesh,	
					unsigned int obstacleMap,
					unsigned int flowVelocity,
					unsigned int flowPressure);

	
};

// normal: normal of plane
// center: any point on the plane
// rayDir: direction of ray
// rayOrigin: ray starting position
// t: rayOrigin + rayDir * t  is the intersection point, if it exists
bool RayPlaneIntersection(glm::vec3 normal, glm::vec3 center, glm::vec3 rayDir, glm::vec3 rayOrigin, float &t);