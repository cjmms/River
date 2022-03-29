#pragma once

// Project includes
// ...

// System includes:
#include <vector>
#include <memory>

// Third party includes:
#include <glm/glm.hpp>


// TODO: Create an RNG helper class.

/*
	This class is for grid-based. 2D fluid simulation. It is based on the following link:
	https://www.dgp.toronto.edu/public_user/stam/reality/Research/pdf/GDC03.pdf
	Authors:
	 - Jarred Eagley, jarred.e@digipen.edu, 60001920
*/

enum class FluidBoundryType
{
	OPEN,
	CLOSED,
	LOOP
};

struct FluidCell
{
	glm::vec2 velocity = glm::vec2(0.0f);
	float density = 1.0f;
};

class FluidContainer
{
public:
	FluidContainer(int width, int height) : gridResolution{width, height}
	{}

	~FluidContainer()
	{
		destroyGrid();
	}

	void integrate(const float& dt);

	glm::vec2 getVelocityAtPosition();
	float getDensityAtPosition();

	void setContainerBoundry(const FluidBoundryType& type);
	const FluidBoundryType& getContainerBoundry();

	void setGridWidth(int width);
	int getGridWidth();
	void setGridHeight(int height);
	int getGridHeight();
	void setGridResolution(glm::ivec2 resolution);
	const glm::ivec2& getGridResolution();

	//void setB

private: // Methods
	void destroyGrid();
	void buildGrid();

	void diffuse(const float& dt);
	
private: // Variables
	glm::ivec2 gridResolution;
	FluidBoundryType containerBoundry = FluidBoundryType::OPEN;

	bool isGridInitialized = false;
	std::vector<std::vector<std::shared_ptr<FluidCell>>> grid;

};

