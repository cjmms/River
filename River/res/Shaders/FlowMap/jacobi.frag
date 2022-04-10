#version 450 core

/*
	The Jacobi Iterator
		Jacobian iteration is described as "the meat of the algorithm."
	It is an iterative method of approximating the solution to
	linear systems.

		Note: This shader should be re-ran iteratively many times to 
	approach a reasonable result. 40 times seems to be the standard.
*/

// inputs
// ...

// outputs
out vec4 fragColor; // TODO: Can this be reduced for better efficiency?

// uniforms
layout(location = 0) uniform sampler2D uObstacleMap;
layout(location = 1) uniform sampler2D uPressure;
// layout(location = 2) uniform sampler2D uVelocity;
layout(location = 3) uniform sampler2D uDivergence;

uniform float uAlpha, uInverseBeta;


void main()
{
	const ivec2 T = ivec2(gl_FragCoord.xy);

	// Useful reference:
	// https://gamedev.stackexchange.com/questions/66448/how-do-opengls-texelfetch-and-texture-differ

	// Fetch the neighboring pressure pixels:
	vec4 pUp	 = texelFetchOffset(uPressure, T, 0, ivec2(0,  1));
	vec4 pDown   = texelFetchOffset(uPressure, T, 0, ivec2(0, -1));
	vec4 pRight  = texelFetchOffset(uPressure, T, 0, ivec2( 1, 0));
	vec4 pLeft   = texelFetchOffset(uPressure, T, 0, ivec2(-1, 0));
	vec4 pCenter = texelFetch(uPressure, T, 0);
		
	// Fetch neighboring obstacle map pixels:
	const bool oUp	   = texelFetchOffset(uObstacleMap, T, 0, ivec2(0,  1)).x > 0;
	const bool oDown   = texelFetchOffset(uObstacleMap, T, 0, ivec2(0, -1)).x > 0;
	const bool oRight  = texelFetchOffset(uObstacleMap, T, 0, ivec2( 1, 0)).x > 0;
	const bool oLeft   = texelFetchOffset(uObstacleMap, T, 0, ivec2(-1, 0)).x > 0;
	const bool oCenter = texelFetch(uObstacleMap, T, 0).x > 0;

	// Use center pressure for solid cells:
	if (oUp)	pUp    = pCenter;
	if (oDown)	pDown  = pCenter;
	if (oRight)	pRight = pCenter;
	if (oLeft)	pLeft  = pCenter;

	// Fetch the divergence for this pixel:
	vec4 divergence = texelFetch(uDivergence, T, 0);

	fragColor = (pLeft + pRight + pUp + pDown + (uAlpha * divergence)) * uInverseBeta;
}
