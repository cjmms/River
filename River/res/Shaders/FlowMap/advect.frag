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
uniform sampler2D uObstacleMap;
uniform sampler2D uVelocity;
// layout(location = 2) uniform sampler2D uPressure;
//layout(location = 3) uniform sampler2D uDivergence;

uniform sampler2D uSoureTexture; // Advection is a generalized step ran on each parameter, so this is a generalized uniform location.

//uniform vec2 uInverseSize;
uniform float uDeltaTime;
uniform float uDissipation;

uniform vec2 uDstScale = vec2(512);


void main()
{
	//vec2 uv = gl_FragCoord.xy / uDstScale;
	// Check the solidity at this pixel:
	vec2 uInverseSize = vec2(1.f/uDstScale.x, 1.f/uDstScale.y);					// UGLY HARDCODING!!!
	const bool isSolid = texture(uObstacleMap, gl_FragCoord.xy*uInverseSize).x > 0;
	if (isSolid)
	{
		fragColor = vec4(0.0f);
		return;
	}

	// Compute the advection at this pixel:
	const vec2 u = texture(uVelocity, uInverseSize * gl_FragCoord.xy).xy; // velocity at the current cell
	const vec2 coord = uInverseSize * (gl_FragCoord.xy - uDeltaTime * u); // coordinate after moving based on velocity

	//fragColor = uDissipation * texture(uSoureTexture, coord);
	fragColor =  texture(uSoureTexture, coord);
}
