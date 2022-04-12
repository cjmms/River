#include "Mesh.h"
#include <cstdlib>

#include <GL/glew.h>

const unsigned int NUM_PATCH_PTS = 4;

WaveParticleMesh::WaveParticleMesh(int numberOfParticles)
	: size(numberOfParticles)
{
	for (int i = 0; i < numberOfParticles; ++i)
	{
		// random value, range: [-1, 1]
		glm::vec2 pos = { rand() / (float)RAND_MAX * 2 - 1, rand() / (float)RAND_MAX * 2 - 1};
		glm::vec2 dir = { rand() / (float)RAND_MAX * 2.f - 1.f, rand() / (float)RAND_MAX * 2.f - 1.f };
		float height = rand() / (float)RAND_MAX * 0.1 + 0.2;
		float speed = rand() / (float)RAND_MAX * 0.05 + 0.1;
		float radius = rand() / (float)RAND_MAX;

		particleList.push_back({pos, dir, height, speed, radius});
	}


	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, particleList.size() * sizeof(WaveParticle), particleList.data(), GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	// position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(WaveParticle), (void*)0);

	// dir
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(WaveParticle), (void*)(2 * sizeof(float)));

	// height
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(WaveParticle), (void*)(4 * sizeof(float)));

	// speed
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(WaveParticle), (void*)(5 * sizeof(float)));

	// radius
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(WaveParticle), (void*)(6 * sizeof(float)));
	
	glBindVertexArray(0);
}




void ObstacleMesh::AddObstacle(glm::vec3 pos)
{
	obstacleList.push_back(Obstacle{pos});
}



void ObstacleMesh::Bind()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, obstacleList.size() * sizeof(Obstacle), obstacleList.data(), GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	// position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Obstacle), (void*)0);
}



WaterMesh::WaterMesh(unsigned int size)
{
	float quadPatchVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords    
		-1.0f, -1.0f,     0.0f, 0.0f,
		1.0f, -1.0f,     1.0f, 0.0f,
		-1.0f,  1.0f,     0.0f, 1.0f,
		 1.0f,  1.0f,     1.0f, 1.0f
	};

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadPatchVertices), &quadPatchVertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);

	glBindVertexArray(0);
}