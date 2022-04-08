#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// wave particle is a random position inside a square from [0, 1]
struct WaveParticle
{
	glm::vec2 pos;
	glm::vec2 dir;
	float height;
	float speed;
	float radius;
};

struct Obstacle
{
	glm::vec3 pos;
};



class WaveParticleMesh
{
public:
	// init mesh with input size, each particle has random pos, random, dir, random height, etc.
	WaveParticleMesh(int numberOfParticles);

	unsigned int size;	// number of wave particles of current mesh
	std::vector<WaveParticle> particleList;

	// no index buffer, simply because I'm lazy
	unsigned int VBO, VAO;
};



class ObstacleMesh
{
public:
	// init mesh with input size, each particle has random pos, random, dir, random height, etc.
	ObstacleMesh() = default;

	void AddObstacle(glm::vec3 pos);

	void Bind();

	inline int size() const { return obstacleList.size(); }

	std::vector<Obstacle> obstacleList;

	// no index buffer, simply because I'm lazy
	unsigned int VBO, VAO;
};
