#pragma once

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

class FluidGrid
{
public:
	FluidGrid(int width, int height) : gridResolution{width, height}
	{}

	~FluidGrid()
	{}

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
	
	void recomputeContainer(); // I dont know if I'll need this.

	
private: // Variables
	glm::ivec2 gridResolution;

	FluidBoundryType containerBoundry = FluidBoundryType::OPEN;

};

