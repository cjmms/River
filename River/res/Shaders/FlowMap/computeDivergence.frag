#version 450 core

// inputs
// ...

// outputs
out float fragColor; // Interesting note; outputs a single float.

// uniforms
uniform sampler2D uObstacleMap;
uniform sampler2D uVelocity;
// layout(location = 2) uniform sampler2D uPressure;
// layout(location = 3) uniform sampler2D uDivergence;

uniform float uHalfInvCellSize; // This seems oddly specific

uniform vec2 uGridScale = vec2(512); // TODO! Should be passed in! But ill keep it harcoded for now...

//uniform vec2 uInvObstacleScale;

void main()
{
	const ivec2 T = ivec2(gl_FragCoord.xy);
	
	// Fetch neighboring velocities:
	vec2 vUp	= texelFetchOffset(uVelocity, T, 0, ivec2( 0, 1)).xy;
	vec2 vDown  = texelFetchOffset(uVelocity, T, 0, ivec2( 0,-1)).xy;
	vec2 vRight = texelFetchOffset(uVelocity, T, 0, ivec2( 1, 0)).xy;
	vec2 vLeft  = texelFetchOffset(uVelocity, T, 0, ivec2(-1, 0)).xy;
	
	// Fetch neighboring obstacles:
	const bool oUp	   = texelFetchOffset(uObstacleMap, T, 0, ivec2(0,  1)).x > 0;
	const bool oDown   = texelFetchOffset(uObstacleMap, T, 0, ivec2(0, -1)).x > 0;
	const bool oRight  = texelFetchOffset(uObstacleMap, T, 0, ivec2( 1, 0)).x > 0;
	const bool oLeft   = texelFetchOffset(uObstacleMap, T, 0, ivec2(-1, 0)).x > 0;

	// Obstacle velocities are not implemented, so use velocity of 0 for solid cells:
	//if (oUp)    vUp    = vec2(0.0f);
	//if (oDown)  vDown  = vec2(0.0f);
	//if (oRight) vRight = vec2(0.0f);
	//if (oLeft)  vLeft  = vec2(0.0f);

	if (oUp)    vUp    = -vUp;
	if (oDown)  vDown  = -vDown;
	if (oRight) vRight = -vRight;
	if (oLeft)  vLeft  = -vLeft;

	// Boundry conditions:
	if (T.x <= 0) vLeft.x = -vLeft.x;
	if (T.x >= uGridScale.x) vRight.x = -vRight.x;
	if (T.y <= 0) vDown.y = -vDown.y;
	if (T.y >= uGridScale.y) vUp.y = -vUp.y;

	fragColor = uHalfInvCellSize * (vRight.x - vLeft.x + vUp.y - vDown.y);
}
