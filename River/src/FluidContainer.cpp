#include "FluidContainer.h"


/// PUBLIC FUNCTIONS ///
#pragma region public

void FluidContainer::integrate(const float& dt)
{
	// ...
}

glm::vec2 FluidContainer::getVelocityAtPosition()
{
	// ...
}

float FluidContainer::getDensityAtPosition()
{
	// ...
}

void FluidContainer::addVelocityAtPosition(glm::ivec2& position, glm::vec2& vel)
{
	// Note: I'm not going to bother with validity checking.
	grid[position.x][position.y]->velocity += vel;
}

void FluidContainer::addDensityAtPosition(glm::ivec2& position, float& density)
{
	grid[position.x][position.y]->density += density;
}


#pragma endregion

#pragma region setters/getters

void FluidContainer::setContainerBoundry(const FluidBoundryType& type)
{
	this->containerBoundry = type;
}
const FluidBoundryType& FluidContainer::getContainerBoundry()
{
	return this->containerBoundry;
}

// in the future, these below functions might need to call
//		some kind of rebuild function.. --j
void FluidContainer::setGridWidth(int width)
{
	this->gridResolution.x = width;
}
int FluidContainer::getGridWidth()
{
	// might change this to return by const ref. --j
	return this->gridResolution.x;
}

void FluidContainer::setGridHeight(int height)
{
	this->gridResolution.y = height;
}
int FluidContainer::getGridHeight()
{
	return this->gridResolution.y;
}

void FluidContainer::setGridResolution(glm::ivec2 resolution)
{
	this->gridResolution = resolution;
}
const glm::ivec2& FluidContainer::getGridResolution()
{
	return this->gridResolution;
}

#pragma endregion

/// PRIVATE FUNCTIONS ///
#pragma region private


/*
	TODO: REWRITE THE GRID HELPER FUNCTIONS TO WORK
	IN TERMS OF [X, Y] RATHER THAN [R, C]!!
*/

void FluidContainer::destroyGrid()
{
	for (int r = 0; r < grid.size(); ++r)
	{
		grid[r].clear();
	}
	grid.clear();
}

void FluidContainer::buildGrid()
{
	// For now I'll just have this overwrite any existing grid...
	if (isGridInitialized) destroyGrid;

	grid.reserve(gridResolution.y); // Reserve the rows.
	for (int r = 0; r < getGridWidth(); ++r)
	{
		auto& row = grid[r];
		row.reserve(gridResolution.x); // Reserve the columns in this row.
	}

	isGridInitialized = true;
}


/// <summary>
/// Things suspended in an incompressible fluid will
///		diffuse into neighboring fluid cells.
/// Uses the linear system solver from:
///		https://www.dgp.toronto.edu/public_user/stam/reality/Research/pdf/GDC03.pdf
/// </summary>
void FluidContainer::diffuse(const float& dt)
{
	const float& width = getGridWidth();
	const float& height = getGridHeight();
	
	const float a = dt * fluidProperties.diffusionRate * width * height;
	const float denominator = 1 + 4 + a;

	// Applies the Gauss-Seidel linear approximator method to work backwards.
	constexpr int ITR = 20;
	for (int k = 0; k < ITR; ++k)
	{
		for (int x = 1; x < width; ++x) 
		{
			for (int y = 1; y < height; ++y) 
			{
				grid[x][y]->velocity;
			}
		}
		// set bounds funct shoudl go here.
		// ...
	}

}

void FluidContainer::project() 
{

}

void FluidContainer::advect() 
{

}



#pragma endregion