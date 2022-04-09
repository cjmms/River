#version 450 core

/*
	Advect:
		"Copies a quantity from a neighboring cell into the current cell; 
	projects the current velocity backwards to find the incoming value. 
	This is used for any type of quantity, including density, 
	temperature, and velocity itself."

		Advection is the process of moving properties of a fluid, such as 
	velocity along according to the timestep and velocity field. It is
	responsible for a lot of the movement in the fluid domain.
*/

// inputs
// ...

// outputs
out vec4 fragColor;

// uniforms
uniform sampler2D uVelocityTexture;
uniform sampler2D uSoureTexture;
uniform sampler2D uObstacleMap;

uniform vec2 uInverseSize;
uniform float uDeltaTime;
uniform float uDissipation;


void main()
{
	// Check the solidity at this pixel:
	const bool isSolid = texture(uObstacleMap, gl_FragCoord.xy).x > 0;
	if (isSolid)
	{
		fragColor = vec4(0.0f);
		return;
	}

	// Compute the advection at this pixel:
	const vec2 u = texture(uVelocityTexture, uInverseSize * gl_FragCoord.xy).xy;
	const vec2 coord = uInverseSize * (gl_FragCoord.xy - (uDeltaTime * u));

	fragColor = uDissipation * texture(uSoureTexture, coord);
}