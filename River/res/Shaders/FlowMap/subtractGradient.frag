#version 450 core

// inputs
// ...

// outputs
out vec2 fragColor; // Vec2. TODO: Is this right?

// uniforms
uniform sampler2D uObstacleMap;
uniform sampler2D uVelocity;
uniform sampler2D uPressure;
//layout(location = 3) uniform sampler2D uDivergence;

uniform float uGradientScale;

uniform vec2 uGridScale = vec2(512);

void main()
{
	const ivec2 T = ivec2(gl_FragCoord.xy);

	const bool isSolid = texelFetch(uObstacleMap, T, 0).x > 0;
	if (isSolid)
	{
		fragColor = vec2(0); // Not doing obstacle veloctiy, so just set vel 0
		return;
	}

	// Fetch the pressure of neighboring pixels:
	float pUp	  = texelFetchOffset(uPressure, T, 0, ivec2(0,  1)).x;
	float pDown   = texelFetchOffset(uPressure, T, 0, ivec2(0, -1)).x;
	float pRight  = texelFetchOffset(uPressure, T, 0, ivec2( 1, 0)).x;
	float pLeft   = texelFetchOffset(uPressure, T, 0, ivec2(-1, 0)).x;
	float pCenter = texelFetch(uPressure, T, 0).x;

	// Some codebases have the yz of the obstacle maps encode a velocity for the obstacles.
	// I chose not to do that.
	// Fetch the solidity of neighboring pixels:
	const bool oUp	   = texelFetchOffset(uObstacleMap, T, 0, ivec2(0,  1)).x > 0;
	const bool oDown   = texelFetchOffset(uObstacleMap, T, 0, ivec2(0, -1)).x > 0;
	const bool oRight  = texelFetchOffset(uObstacleMap, T, 0, ivec2( 1, 0)).x > 0;
	const bool oLeft   = texelFetchOffset(uObstacleMap, T, 0, ivec2(-1, 0)).x > 0;
	// const bool oCenter = texelFetch(uObstacleMap, T, 0).x > 0;

	// const vec2 obstV = vec2(0); This is the obstacle velocity, which I'm not using.
	
	vec2 vMask = vec2(1);
	if (oUp)
	{
		pUp = pCenter;
		vMask.y = 0;
	}
	if (oDown)
	{
		pDown = pCenter;
		vMask.y = 0;
	}
	if (oRight)
	{
		pRight = pCenter;
		vMask.x = 0;
	}
	if (oLeft)
	{
		pLeft = pCenter;
		vMask.x = 0;
	}

	// Boundry conditions:
	if (T.x <= 0) pLeft = pCenter;
	if (T.x >= uGridScale.x) pRight = pCenter;
	if (T.y <= 0) pDown = pCenter;
	if (T.y >= uGridScale.y) pUp = pCenter;

	// Enforce free-slip boundry condition.
	vec2 oldVelocity = texelFetch(uVelocity, T, 0).xy;
	vec2 gradient = vec2(pRight - pLeft, pUp - pDown) * 0.06f;//uGradientScale; // HARDCODED FOR TIME'S SAKE
	vec2 newVelocity = oldVelocity - gradient;
	
	fragColor = (vMask * newVelocity); // Add obstacle velocity here too if we end up wanting it.
}
