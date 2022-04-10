#version 450 core

/*
	Preserves more swirlies in the final result.
	(Not to be immediately implemented, make sure everything else works first!)
*/

// inputs
// ...

// outputs
out vec4 fragColor; 

// uniforms
uniform sampler2D uPressure;
uniform sampler2D uDivergence;
uniform sampler2D uObstacleMap;

//uniform float uDeltaTime;
//uniform float uCellSize;

uniform float uAlpha;
uniform float uInvBeta;

void main()
{
	const ivec2 T = ivec2(gl_FragCoord.xy);
	
	/*
	const bool isSolid = texelFetch(uObstacleMap, T, 0).x > 0;
	if (isSolid)
	{
		fragColor = vec2(0); // Not doing obstacle veloctiy, so just set vel 0
		return;
	}
	*/

	// Fetch the pressure of neighboring pixels:
	float pUp	  = texelFetchOffset(uPressure, T, 0, ivec2(0,  1)).x;
	float pDown   = texelFetchOffset(uPressure, T, 0, ivec2(0, -1)).x;
	float pRight  = texelFetchOffset(uPressure, T, 0, ivec2( 1, 0)).x;
	float pLeft   = texelFetchOffset(uPressure, T, 0, ivec2(-1, 0)).x;
	float pCenter = texelFetch(uPressure, T, 0).x;

	// Fetch the solidity of neighboring pixels:
	const bool oUp	   = texelFetchOffset(uObstacleMap, T, 0, ivec2(0,  1)).x > 0;
	const bool oDown   = texelFetchOffset(uObstacleMap, T, 0, ivec2(0, -1)).x > 0;
	const bool oRight  = texelFetchOffset(uObstacleMap, T, 0, ivec2( 1, 0)).x > 0;
	const bool oLeft   = texelFetchOffset(uObstacleMap, T, 0, ivec2(-1, 0)).x > 0;

	vec2 vMask = vec2(1);
	if (oUp)    pUp = pCenter;
	if (oDown)  pDown = pCenter;
	if (oRight) pRight = pCenter;
	if (oLeft)  pLeft = pCenter;

	// ...
	int X = 100; // TODO: Pass in the texture scale, or otherwise retrieve it somehow!!
	if (T.x <= 0) pUp    = pCenter;
	if (T.x >= X) pDown  = pCenter;
	if (T.y <= 0) pRight = pCenter;
	if (T.y >= X) pLeft  = pCenter;

	vec4 dCenter = texelFetch(uDivergence, T, 0);


	fragColor = (pLeft + pRight + pDown + pUp + uAlpha * dCenter) * uInvBeta;
}
