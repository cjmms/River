#include "FluidContainer.h"


/// PUBLIC FUNCTIONS ///
#pragma region public

void FluidContainer::integrate(const float& dt)
{
	// ...
}

glm::vec2 FluidContainer::getVelocityAtPosition(const glm::ivec2& position) const
{
	return v.getData(position.x+1, position.y+1);
}

float FluidContainer::getDensityAtPosition(const glm::ivec2& position) const
{
	return density.getData(position.x+1, position.y+1);
}


// Note: I'm not going to bother with validity checking.
void FluidContainer::addVelocityAtPosition(const glm::ivec2& position, glm::vec2& vel)
{
	v.getDataReference(position.x+1, position.y+1) += vel;
}

void FluidContainer::addDensityAtPosition(const glm::ivec2& position, float& d)
{
	density.getDataReference(position.x+1, position.y+1) += d;
}


#pragma endregion

#pragma region setters/getters

//void FluidContainer::setContainerBoundry(const FluidBoundryType& type)
//{
//	this->containerBoundry = type;
//}
//const FluidBoundryType& FluidContainer::getContainerBoundry()
//{
//	return this->containerBoundry;
//}

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


void FluidContainer::buildGrid()
{
	float w = getGridWidth() + 2;	// Add 2 for padding (borders)
	float h = getGridHeight() + 2;	// Add 2 for padding (borders)
	v		= Array2D<glm::vec2>(w, h);
	v_prev  = Array2D<glm::vec2>(w, h);
	density = Array2D<float>(w, h);
	s		= Array2D<float>(w, h);

	//isGridInitialized = true;
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