#pragma once

// TODO: Import GLM

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
	FluidGrid();
	~FluidGrid();

	void getVelocityAtPosition();
	void getDensityAtPosition();

	//void setB

private:
	float gridWidth;		// TODO: Getters and setters for this.
	float gridHeight;

};

