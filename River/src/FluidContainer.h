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
	const T getData(const int& x, const int& y) const
	{
		assert(x < width&& y < height);
		return data[(y * width) + x];
	}

	// Indexed rather than by coordinate. Use with caution.
	const T getData(const int& i) const
	{
		return data[i];
	}

	// Get a modifyable reference.
	T& getDataReference(const int& x, const int& y)
	{
		assert(x < width&& y < height);
		return data[(y * width) + x];
	}

	// Use with caution; directly accesses data.
	T& getDataReference(const int& i)
	{
		return data[i];
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
		printf("Fluid Container built.\n");
	}

	~FluidContainer() 
	{
		delete vx;
		delete vy;
		delete vx_prev;
		delete vy_prev;
		delete density;
		delete s;
		printf("Fluid Container deleted.\n");
	}

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

	// Private utility function for swapping arrays around.
	template<typename T>
	void swapPtr(T* a, T* b)
	{
		T* tmp = a;
		a = b;
		b = tmp;
	}

	// Simulation functions:

	void densityStep();
	void VelocityStep();

	template<typename T>
	void addSource(Array2D<T>* x, Array2D<T>* s, const float& dt);
	
	template<typename T>
	void diffuse(Array2D<T>* x, Array2D<T>* x0, const int& b, const float& dt);
	
	void project();
	
	void advect();

	template<typename T>
	void setBound(int b, Array2D<T>* x);

private: // Variables
	glm::ivec2 gridResolution;

	// Things that were previously inside fluidCell, now each gets its own array:
	// To cut down on templating, velocities are stored as floats rather than vectors...
	Array2D<float>* vx;
	Array2D<float>* vy;
	Array2D<float>* vx_prev;
	Array2D<float>* vy_prev;
	Array2D<float>* density;
	Array2D<float>* s;

};

