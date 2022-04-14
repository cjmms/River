#pragma once
#include "Mesh.h"
#include "Shader.h"

extern const int window_width;
extern const int window_height;

#define FIXED_DELTA_TIME 0.5f

class FBO
{
public:
	FBO(unsigned int width, unsigned int height);

	// clear both color buffer and depth buffer
	void Clear();

	void AddTarget(unsigned int width, unsigned int height);

	unsigned int ID, DepthBuffer, ColorBuffer1, ColorBuffer2;
};

class PingPong
{
public:
	PingPong(FBO* first, FBO* second) : ping{first}, pong{second}
	{}
	PingPong(int width, int height)
	{
		ping = new FBO(width, height);
		pong = new FBO(width, height);
	}
	~PingPong()
	{
		if (ping) delete ping;
		if (pong) delete pong;
	}
	FBO* ping = nullptr;
	FBO* pong = nullptr;

	void Swap()
	{
		std::swap(ping, pong);
	}
	void AddTargetToBoth(int width, int height)
	{
		ping->AddTarget(width, height);
		pong->AddTarget(width, height);
	}
};

struct Setting
{
	int blurSize = 50;
	int particleSize = 1;
	float dx = 0.01;
	float dz = 0.01;

	int tessellationFactor = 3;
	bool enableWireframeMode = false;
	float heightFactor = 0.1f;

	int seletectedRenderPass = 5;

	// lighting
	float waterDepth = 9.3f;
	float extinctionCoeff  = 0.242f;
	glm::vec3 waterBedColor = glm::vec3(0.4, 0.3, 0.2);
	float FoamTurbulance = 0.043;

	float timeScale = 2;
	bool enableNormalMap = false;

	float brushSize = 1.0;
	float obstacleHeightFactor = 2.0;
	int obstacleTessellationFactor = 20;
};



class Render
{
public:
	// wave particle visialization, only dots
	Shader waveParticleShader{ "res/Shaders/WaveParticle.vert", "res/Shaders/WaveParticle.frag" };

	// horizontal blur, used to generate height map base on wave particles
	// technically, it is blurring.
	// from the project perspective, it's one of two passes of height map reconstruction base on wave equations
	Shader horozontalBlur{ "res/Shaders/Blur.vert", "res/Shaders/HorizontalBlur.frag" };

	// vertical blur, used to generate height map base on wave particles
	// the second pass of wave reconstruction
	Shader verticalBlur{ "res/Shaders/Blur.vert", "res/Shaders/VerticalBlur.frag" };


	Shader quadShader{ "res/Shaders/Blur.vert", "res/Shaders/Quad.frag" };

	Shader waveMeshShader{ "res/Shaders/wave.vert",
							"res/Shaders/wave.tesc",
							"res/Shaders/wave.tese",
							"res/Shaders/wave.frag" };

	Shader createObstacleShader{ "res/Shaders/CreateObstacle.vert", "res/Shaders/CreateObstacle.frag" };

	Shader obstacleBlurHShader{ "res/Shaders/Blur.vert", "res/Shaders/ObstacleBlur_H.frag" };
	Shader obstacleBlurVShader{ "res/Shaders/Blur.vert", "res/Shaders/ObstacleBlur_V.frag" };

	Shader renderObstacleShader{ "res/Shaders/wave.vert",
									"res/Shaders/wave.tesc",
									"res/Shaders/RenderObstacle.tese",
									"res/Shaders/RenderObstacle.frag" };

	FBO obstacleBlurFBO{ window_width , window_height };
	unsigned int impulseMapTexture = -1;


	Shader flowAdvect{ "res/Shaders/FlowMap/simple.vert", "res/Shaders/FlowMap/advect.frag" };
	Shader flowComputeDivergence{ "res/Shaders/FlowMap/simple.vert", "res/Shaders/FlowMap/computeDivergence.frag" };
	Shader flowJacobi{ "res/Shaders/FlowMap/simple.vert", "res/Shaders/FlowMap/jacobi.frag" };
	Shader flowSubtractGradient{ "res/Shaders/FlowMap/simple.vert", "res/Shaders/FlowMap/subtractGradient.frag" };
	Shader flowAdder{ "res/Shaders/FlowMap/simple.vert", "res/Shaders/FlowMap/addTextures.frag" };

	glm::vec2 fluidGridScale = { 512.f, 512.f };
	glm::vec2 fluidInvScale = { 1.f / 512.f, 1.f / 512.f };
	float cellSize = 1.25f; // Pretty much just a constant setting relating to the fluids' behavior.
	float gradientScale = 1.125 / cellSize;
	bool impulseFieldEnabled = true;

	glm::vec2 flowDirection = {1.0f, 0.0f};
	float flowMultiplier = 1.0f;
	float flowIntensity = 1.0f;

public: 
	Render();


	void UpdateFlowMap(Quad& quad, FBO* obstacleFBO, PingPong* velocity, PingPong* pressure, FBO* divergence);

	void RenderWaveParticle(WaveParticleMesh& waveParticleMesh, unsigned int flowMap, unsigned int fbo);

	void HorizontalBlur(Quad& quad, unsigned int inputTexture, unsigned int fbo);
	void VerticalBlur(Quad& quad, unsigned int f123, unsigned int f45v, unsigned int fbo);

	void RenderWaveMesh(WaterMesh& waterMesh, unsigned int flowMap, unsigned int divergenceMap, unsigned int pressureMap,
		unsigned int irradianceMap, unsigned int obstacleMap, unsigned int skybox, unsigned int deviation, unsigned int gradient, unsigned int fbo);

	void RenderObstacleHeightMap(Quad& quad, unsigned int fbo);

	void RenderObstacles(WaterMesh& waterMesh, unsigned int heightMap, unsigned int fbo);

	// Blur obstacle position
	void ObstacleBlur(Quad& quad, unsigned int ObstaclePosMap, unsigned int fbo);

	void DebugDraw(Quad& quad,
					unsigned int particleMap, 
					unsigned int f123, 
					unsigned int f45v, 
					unsigned int deviation,
					unsigned int gradient,
					unsigned int waveMesh,
					unsigned int obstaclePosMap,
					unsigned int obstacleMap,
					unsigned int flowDivergence,
					unsigned int flowVelocity,
					unsigned int flowPressure
	);

	
// changed to public for debuging purposes, change it to private when everythings work
public:
//private:
	// (Velocity and pressure are two color attachments on the same fbo)
	void AdvectHelper(Quad& quad, FBO* velocity, FBO* obstacles, FBO* src, FBO* dst, float dissipation);
	void JacobiHelper(Quad& quad, FBO* pressure, FBO* divergence, FBO* obstacles, FBO* dst);
	void SubtractGradientHelper(Quad& quad, FBO* velocity, FBO* pressure, FBO* obstacles, FBO* dst);
	void ComputeDivergenceHelper(Quad& quad, FBO* velocity, FBO* obstacles, FBO* dst);
	void ApplyExternalFlow(Quad& quad, FBO* velocity, unsigned int srcTex, float multiplier);
};

// normal: normal of plane
// center: any point on the plane
// rayDir: direction of ray
// rayOrigin: ray starting position
// t: rayOrigin + rayDir * t  is the intersection point, if it exists
bool RayPlaneIntersection(glm::vec3 normal, glm::vec3 center, glm::vec3 rayDir, glm::vec3 rayOrigin, float &t);
