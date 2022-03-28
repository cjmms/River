#include "FluidGrid.h"


/// PUBLIC FUNCTIONS ///
#pragma region public

void FluidGrid::integrate(const float& dt)
{
	// ...
}

glm::vec2 FluidGrid::getVelocityAtPosition()
{
	// ...
}

float FluidGrid::getDensityAtPosition()
{
	// ...
}

#pragma endregion

#pragma region setters/getters

void FluidGrid::setContainerBoundry(const FluidBoundryType& type)
{
	this->containerBoundry = type;
}
const FluidBoundryType& FluidGrid::getContainerBoundry()
{
	return this->containerBoundry;
}

// in the future, these below functions might need to call
//		some kind of rebuild function.. --j
void FluidGrid::setGridWidth(int width)
{
	this->gridResolution.x = width;
}
int FluidGrid::getGridWidth()
{
	// might change this to return by const ref. --j
	return this->gridResolution.x;
}

void FluidGrid::setGridHeight(int height)
{
	this->gridResolution.y = height;
}
int FluidGrid::getGridHeight()
{
	return this->gridResolution.y;
}

void FluidGrid::setGridResolution(glm::ivec2 resolution)
{
	this->gridResolution = resolution;
}
const glm::ivec2& FluidGrid::getGridResolution()
{
	return this->gridResolution;
}

#pragma endregion

/// PRIVATE FUNCTIONS ///
#pragma region private

void FluidGrid::destroyGrid()
{
	for (int r = 0; r < grid.size(); ++r)
	{
		grid[r].clear();
	}
	grid.clear();
}

void FluidGrid::buildGrid()
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


#pragma endregion