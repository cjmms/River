#pragma once
#include "Mesh.h"
#include "Shader.h"

extern const int window_width;
extern const int window_height;

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
		//if (ping) delete ping;
		//if (pong) delete pong;
	}
	FBO* ping = nullptr;
	FBO* pong = nullptr;

	void Swap()
	{
		FBO* temp = ping;
		pong = ping;
		ping = temp;
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

	float brushSize = 1;
	float obstacleHeightFactor = 1;
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

	Shader obstacleBlurHShader{ "res/Shaders/Blur.vert", "res/Shaders/ObstacleBlur_H.frag" };
	Shader obstacleBlurVShader{ "res/Shaders/Blur.vert", "res/Shaders/ObstacleBlur_V.frag" };

	Shader renderObstacleShader {	"res/Shaders/wave.vert",
									"res/Shaders/wave.tesc",
									"res/Shaders/RenderObstacle.tese",
									"res/Shaders/RenderObstacle.frag" };

	unsigned int quadVAO, quadVBO;

	FBO obstacleBlurFBO{ window_width , window_height };
	
	
	Shader flowAdvect			{"res/Shaders/FlowMap/simple.vert", "res/Shaders/FlowMap/advect.frag"};
	Shader flowComputeDivergence{"res/Shaders/FlowMap/simple.vert", "res/Shaders/FlowMap/computeDivergence.frag"};
	Shader flowJacobi			{"res/Shaders/FlowMap/simple.vert", "res/Shaders/FlowMap/jacobi.frag"};
	Shader flowSubtractGradient {"res/Shaders/FlowMap/simple.vert", "res/Shaders/FlowMap/subtractGradient.frag"};

	glm::ivec2 fluidGridScale = {512, 512};
	glm::vec2 fluidInvScale = {1.f / 512.f, 1.f / 512.f};
	float cellSize = 1.25f; // Pretty much just a constant setting relating to the fluids' behavior.
	float gradientScale = 1.125 / cellSize;

public: 
	Render();
	~Render();

	void UpdateFlowMap(FBO* obstacleFBO, PingPong& velocity, PingPong& pressure, FBO* divergence);

	void RenderWaveParticle(WaveParticleMesh& waveParticleMesh, unsigned int fbo);

	void HorizontalBlur(unsigned int inputTexture, unsigned int fbo);
	void VerticalBlur(unsigned int f123, unsigned int f45v, unsigned int fbo);

	void RenderWaveMesh(WaterMesh& waterMesh, unsigned int irradianceMap, unsigned int skybox, unsigned int deviation, unsigned int gradient, unsigned int fbo);

	void RenderObstacleHeightMap(unsigned int fbo);

	void RenderObstacles(WaterMesh& waterMesh, unsigned int heightMap, unsigned int fbo);

	void DrawQuad(unsigned int inputTexture);

	// Blur obstacle position
	void ObstacleBlur(unsigned int ObstaclePosMap, unsigned int fbo);

	void DebugDraw( unsigned int particleMap, 
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
	void AdvectHelper(FBO* velocity, FBO* obstacles, FBO* src, FBO* dst, float dissipation);
	void JacobiHelper(FBO* pressure, FBO* divergence, FBO* obstacles, FBO* dst);
	void SubtractGradientHelper(FBO* velocity, FBO* pressure, FBO* obstacles, FBO* dst);
	void ComputeDivergenceHelper(FBO* velocity, FBO* obstacles, FBO* dst);

	void initQuadMesh();
};

// normal: normal of plane
// center: any point on the plane
// rayDir: direction of ray
// rayOrigin: ray starting position
// t: rayOrigin + rayDir * t  is the intersection point, if it exists
bool RayPlaneIntersection(glm::vec3 normal, glm::vec3 center, glm::vec3 rayDir, glm::vec3 rayOrigin, float &t);
