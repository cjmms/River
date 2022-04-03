#include "FluidContainer.h"


/// PUBLIC FUNCTIONS ///
#pragma region public

void FluidContainer::integrate(const float& dt)
{
	VelocityStep(); // TODO
	densityStep(); // TODO
}

glm::vec2 FluidContainer::getVelocityAtPosition(const glm::ivec2& position) const
{
	return glm::vec2(
		vx->getData(position.x+1, position.y+1), vy->getData(position.x+1, position.y+1)
		);
}

float FluidContainer::getDensityAtPosition(const glm::ivec2& position) const
{
	return density->getData(position.x+1, position.y+1);
}


// Note: I'm not going to bother with validity checking.
void FluidContainer::addVelocityAtPosition(const glm::ivec2& position, glm::vec2& vel)
{
	vx->getDataReference(position.x+1, position.y+1) += vel.x;
	vy->getDataReference(position.x+1, position.y+1) += vel.y;
}

void FluidContainer::addDensityAtPosition(const glm::ivec2& position, float& d)
{
	density->getDataReference(position.x+1, position.y+1) += d;
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
	vx		= new Array2D<float>(w, h);
	vy		= new Array2D<float>(w, h);
	vx_prev = new Array2D<float>(w, h);
	vy_prev = new Array2D<float>(w, h);
	density = new Array2D<float>(w, h);
	s		= new Array2D<float>(w, h);
}


template<typename T>
void FluidContainer::addSource(Array2D<T>* x, Array2D<T>* s, const float& dt)
{
	// Total size of the grid, including boundries:
	const int size = (gridResolution.x + 2) * (gridResolution.y + 2);
	for (int i = 0; i < size; ++i)
	{
		x->getDataReference(i) += dt * s->getData(i);
	}
}

/// <summary>
/// Properties in an incompressible fluid will
///		diffuse into neighboring fluid cells.
/// Uses the linear system solver from:
///		https://www.dgp.toronto.edu/public_user/stam/reality/Research/pdf/GDC03.pdf
/// Note: The 'b' parameter is related to boundry handling.
/// </summary>
template<typename T>
void FluidContainer::diffuse(Array2D<T>* x, Array2D<T>* x0, const int& b, const float& dt)
{
	const float& width = getGridWidth();
	const float& height = getGridHeight();
	
	const float a = dt * fluidProperties.diffusionRate * width * height;
	const float denominator = 1 + 4 + a;

	// Applies the Gauss-Seidel linear approximator method to work backwards.
	constexpr int ITR = 20;
	for (int k = 0; k < ITR; ++k)
	{
		// Excludes boundries, only processes main cells.
		for (int i = 1; i <= width; ++i) 
		{
			for (int j = 1; j <= height; ++j) 
			{
				const T val 
					= (x0->getData(i,j) 
					+ a*( x->getData(i-1, j) + x->getData(i+1, j) + x->getData(i, j-1) + x->getData(i, j+1) ))
					/ (5*a);
				x->setData(val);
			}
		}

		// Handle boundries.
		setBound(b, x);
	}
}

void FluidContainer::project() 
{

}

template<typename T>
void FluidContainer::advect(Array2D<T>* d, Array2D<T>* d0, Array2D<T>* u, Array2D<T>* v, const int& b, const float dt)
{
	const float N = getGridWidth();
	const float M = getGridHeight();
	for (int i = 1; i <= N; ++i) 
	{
		for (int j = 1; j <= M; ++j) 
		{
			const float x = i - dt * N * u->getData(i, j);
			const float y = j - dt * M * v->getData(i, j);
			if (x < 0.5f)
				x = 0.5f;
			else if (x > N + 0.5f)
				x = N + 0.5f;
			if (y < 0.5f)
				y = 0.5f;
			else if (y > M + 0.5f)
				y = M + 0.5f;
			
			const int i0 = (int)x;
			const int i1 = i0+1;
			const int j0 = (int)y;
			const int j1 = j0+1;
			
			const float s1 = x - i0;	// should these really be floats?
			const float s0 = 1.0f - s1;
			const float t1 = y - j0;
			const float t0 = 1.0f - t1;

			const T val 
				= s0 * ( t0*d0->getData(i0, j0) + t1*d0->getData(i0, j1) )
				+ s1 * ( t0*d0->getData(i1, j0) + t1*d0->getData(i1, j1));
			d->setData(val, i, j);
		}
	}
	setbound(b, d);
}

/// <summary>
/// Sets the boundrycells on the outer edge of the grid such that
///		they perfectly counteract their neighbors.
/// </summary>
template<typename T>
void FluidContainer::setBound(const int& b, Array2D<T>* x)
{
	// TODO: Allow this to be set as open.
	
	// horizontal walls:
	for (int i = 1; i <= gridResolution.x; ++i)
	{
		const int N = gridResolution.x;
		x.getDataReference(i, 0)   = x.getData(i, 1) * (b==1 ? -1.0f : 1.0f);
		x.getDataReference(i, N+1) = x.getData(i, N) * (b==1 ? -1.0f : 1.0f);
	}

	// Vertical walls:
	for (int i = 1; i <= gridResolution.y; ++i)
	{
		const int N = gridResolution.y;
		x.getDataReference(0, i)   = x.getData(1, i) * (b == 2 ? -1.0f : 1.0f);
		x.getDataReference(N+1, i) = x.getData(N, i) * (b == 2 ? -1.0f : 1.0f);
	}

	// Corner cases:
	const int N = gridResolution.x; const int M = gridResolution.y;
	x.getDataReference(0, 0)     = 0.5f * x.getData(1, 0)   + x.getData(0, 1);
	x.getDataReference(0, M+1)   = 0.5f * x.getData(1, M+1) + x.getData(0, M);
	x.getDataReference(N+1, 0)   = 0.5f * x.getData(N, 0)   + x.getData(N+1, 1);
	x.getDataReference(N+1, M+1) = 0.5f * x.getData(N, M+1) + x.getData(N+1, M);
}

#pragma endregion