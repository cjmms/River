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
/// Diffusion is the process of high density fluids
/// expanding out into lower density fluids.
/// </summary>
void FluidContainer::diffuse(const float& dt)
{

}

#pragma endregion