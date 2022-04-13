#include "Mesh.h"
#include <cstdlib>

#include <GL/glew.h>
#include "Shader.h"

const unsigned int NUM_PATCH_PTS = 4;

extern const int window_width;
extern const int window_height;

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



WaterMesh::WaterMesh(unsigned int res)
{
	float quadPatchVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords    
		-1.0f, -1.0f,     0.0f, 0.0f,
		1.0f, -1.0f,     1.0f, 0.0f,
		-1.0f,  1.0f,     0.0f, 1.0f,
		 1.0f,  1.0f,     1.0f, 1.0f
	};

	std::vector<float> vertices;

	resolution = res;
	for (unsigned i = 0; i <= resolution - 1; i++)
	{
		for (unsigned j = 0; j <= resolution - 1; j++)
		{
			vertices.push_back(-1.0f + 2.0f / (float)resolution * i);	// v.x
			vertices.push_back(-1.0f + 2.0f / (float)resolution * j);	// v.z
			vertices.push_back(0.0f + 1.0f / (float)resolution * i);	// u
			vertices.push_back(0.0f + 1.0f / (float)resolution * j);	// v

			vertices.push_back(-1.0f + 2.0f / (float)resolution * (i + 1));	// v.x
			vertices.push_back(-1.0f + 2.0f / (float)resolution * j);	// v.z
			vertices.push_back(0.0f + 1.0f / (float)resolution * (i + 1));	// u
			vertices.push_back(0.0f + 1.0f / (float)resolution * j);	// v


			vertices.push_back(-1.0f + 2.0f / (float)resolution * i);	// v.x
			vertices.push_back(-1.0f + 2.0f / (float)resolution * (j + 1));	// v.z
			vertices.push_back(0.0f + 1.0f / (float)resolution * i);	// u
			vertices.push_back(0.0f + 1.0f / (float)resolution * (j + 1));	// v

			vertices.push_back(-1.0f + 2.0f / (float)resolution * (i + 1));	// v.x
			vertices.push_back(-1.0f + 2.0f / (float)resolution * (j + 1));	// v.z
			vertices.push_back(0.0f + 1.0f / (float)resolution * (i + 1));	// u
			vertices.push_back(0.0f + 1.0f / (float)resolution * (j + 1));	// v
		}
	}

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);

	glBindVertexArray(0);
}


void WaterMesh::Draw(Shader &shader)
{
	shader.Bind();
	glBindVertexArray(VAO);
	glDrawArrays(GL_PATCHES, 0, 4 * resolution * resolution);
	glBindVertexArray(0);
	shader.unBind();
}


Quad::Quad()
{
	// init quad
	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}


void Quad::Draw(Shader& shader)
{
	shader.Bind();

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	shader.unBind();
}