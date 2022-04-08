#pragma once

// Project includes
// ...

// System includes:
//#include <vector>
#include <memory>

// Third party includes:
#include <glm/glm.hpp>

#include <GL/glew.h>


// TODO: Create an RNG helper class.

/*
	This class is for grid-based. 2D fluid simulation. It is based on the following link:
	https://www.dgp.toronto.edu/public_user/stam/reality/Research/pdf/GDC03.pdf
	Authors:
	 - Jarred Eagley, jarred.e@digipen.edu, 60001920
*/


template<typename T>
class Array2D
{
private:
	int width, height;
	T* data;

public:
	Array2D() : width{ 0 }, height{ 0 }, data{ nullptr } {}

	Array2D(int w, int h) : width{ w }, height{ h }
	{
		data = new T[width * height];
	}
	~Array2D()
	{
		delete[] data;
	}

	bool isInitialized()   const { return (data == nullptr); }
	const int& getWidth()  const { return width; } 
	const int& getHeight() const { return height; } 
	glm::ivec2 getDim()    const { return { width, height }; } 

	// Can be dangeorus, but needed for flow field generation.
	T* getDataArrayPtr() { return data; };

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

	// Creates a flow map and stores it at the provided texture ID.
	unsigned int createFlowMap() 
	{
		unsigned int texID;
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, getGridWidth(), getGridHeight(), 0, GL_RG, GL_FLOAT, 0);
		updateFlowMap(texID);
		return texID;
	}

	void updateFlowMap(unsigned int textureId)
	{
		float* dataVelX = this->vx_prev->getDataArrayPtr();
		float* dataVelY = this->vy_prev->getDataArrayPtr();

		// Concatenate the x and y velocity into a single data array tha can be used.
		int totalsize = getGridWidth() * getGridHeight() * 2;
		float* totalDataArray = new float[totalsize];
		for (int i = 0; i < totalsize-1; ++i)
		{
			totalDataArray[i * 2] = dataVelX[i];
			totalDataArray[i * 2 + 1] = dataVelY[i];
		}

		// Bind the texture.
		glBindTexture(GL_TEXTURE_2D, textureId);

		// Sub data.
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, getGridWidth(), getGridHeight(), GL_RG, GL_FLOAT, totalDataArray);

		// Deallocate.
		delete[] totalDataArray;
	}

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

	void densityStep(const float& dt)
	{
		// Note: 's' is prev density, I think.
		addSource(s, density, dt);
		swapPtr(density, s);
		diffuse(s, density, 0, dt);
		swapPtr(density, s);
		advect(s, density, vx, vy, 0, dt);
	}
	void VelocityStep(const float& dt)
	{
		addSource(vx, vx_prev, dt); 
		addSource(vy, vy_prev, dt); 
		swapPtr(vx, vx_prev);
		swapPtr(vy, vy_prev);

		diffuse(vx, vx_prev, 1, dt);
		diffuse(vy, vy_prev, 2, dt);
		swapPtr(vx, vx_prev);
		swapPtr(vy, vy_prev);

		// project(); NOT DONE YET!
		advect(vx, vx_prev, vx_prev, vy_prev, 1, dt);
		advect(vy, vy_prev, vx_prev, vy_prev, 2, dt);
		// project(); NOT DONE YET!
	}

	template<typename T>
	void addSource(Array2D<T>* x, Array2D<T>* s, const float& dt);
	
	template<typename T>
	void diffuse(Array2D<T>* x, Array2D<T>* x0, const int& b, const float& dt);
	
	template<typename T>
	void project(Array2D<T>* u, Array2D<T>* v, Array2D<T>* p, Array2D<T>* div);
	
	template <typename T>
	void advect(Array2D<T>* d, Array2D<T>* d0, Array2D<T>* u, Array2D<T>* v, const int& b, const float dt);

	template<typename T>
	void setBound(const int& b, Array2D<T>* x);

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

