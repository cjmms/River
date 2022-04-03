#pragma once

// Project includes
// ...

// System includes:
//#include <vector>
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

/*
struct FluidCell
{
	glm::vec2 velocity = glm::vec2(0.0f);
	glm::vec2 v_prev = glm::vec2(0.0f);
	float density = 1.0f;
	float s;
};
*/

template<typename T>
class Array2D
{
private:
	int width, height;
	T* data;

public:
	Array2D() : width{ 0 }, heigh{ 0 }, data{ nullptr } {}

	Array2D(int w, int h) : width{ w }, height{ h }
	{
		data = new T[width * height];
	}
	~Array2D()
	{
		delete[width * height] data;
	}

	bool isInitialized() { return data == nulptr; } const;
	const int& getWidth() { return width; } const;
	const int& getHeight() { return height; } const;
	glm::ivec2 getDim() { return { width, height }; } const;

	// Get a constant reference to dataat a position.
	const T& getData(const int& x, const int& y) const
	{
		assert(x < width&& y < height);
		return data[(y * width) + x];
	}

	// Get a modifyable reference.
	T& getDataReference(const int& x, const int& y)
	{
		assert(x < width&& y < height);
		return data[(y * width) + x];
	}

	// Set data at a position.
	void setData(const T& input, const int& x, const int& y)
	{
		assert(x < width&& y < height);
		data[(y * width) + x] = input;
	}
};

struct FluidConstants
{
	float viscosity = 1.0f;
	float diffusionRate = 1.0f;
};

class FluidContainer
{
public:
	FluidContainer(int width, int height) : gridResolution{width, height}
	{
		buildGrid();
	}

	~FluidContainer() {}

	void integrate(const float& dt);

	glm::vec2 getVelocityAtPosition(const glm::ivec2& position) const;
	float getDensityAtPosition(const glm::ivec2& position) const;

	void addVelocityAtPosition(const glm::ivec2& position, glm::vec2& vel);
	void addDensityAtPosition(const glm::ivec2& position, float& d);

	//void setContainerBoundry(const FluidBoundryType& type);
	//const FluidBoundryType& getContainerBoundry();

	void setGridWidth(int width);
	int getGridWidth();
	void setGridHeight(int height);
	int getGridHeight();
	void setGridResolution(glm::ivec2 resolution);
	const glm::ivec2& getGridResolution();

public:
	FluidConstants fluidProperties;

private: // Methods

	// Build/destroy helpers
	void buildGrid();

	// Simulation functions
	void diffuse(const float& dt);
	void project();
	void advect();

	template<typename T>
	void setBound(int b, Array2D<T>& x);

private: // Variables
	glm::ivec2 gridResolution;

	// Things that were previously inside fluidCell, now each gets its own array:
	Array2D<glm::vec2> v;		// Velocity
	Array2D<glm::vec2> v_prev;
	Array2D<float> density;  // TODO: HANDLE BOUNDS
	Array2D<float> s;

};

