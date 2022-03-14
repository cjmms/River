#include "Mesh.h"
#include <cstdlib>

#include <GL/glew.h>
#include <iostream>


WaveParticleMesh::WaveParticleMesh(int numberOfParticles)
	: size(numberOfParticles)
{
	//particleList.reserve(size);
	std::cout << "number of particles: " << numberOfParticles << std::endl;

	for (int i = 0; i < numberOfParticles; ++i)
	{
		// random value from [0, 1]
		glm::vec2 pos = { rand() / (float)RAND_MAX * 2 - 1, rand() / (float)RAND_MAX * 2 - 1};
		glm::vec2 dir = { rand() / (float)RAND_MAX * 2.f - 1.f, rand() / (float)RAND_MAX * 2.f - 1.f };
		float height = rand() / (float)RAND_MAX * 0.1 + 0.2;
		float speed = rand() / (float)RAND_MAX * 0.05 + 0.1;
		float radius = rand() / (float)RAND_MAX;

		std::cout << "pos: " << pos.x << ", " << pos.y << std::endl;

		//particleList[i] = { pos, dir, height, speed, radius };
		//particleList[i] = { pos};
		particleList.push_back({pos});
	}


	//
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, particleList.size() * sizeof(WaveParticle), particleList.data(), GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	// position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(WaveParticle), (void*)0);

	/*
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
	*/
	glBindVertexArray(0);
}