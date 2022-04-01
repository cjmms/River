#version 450
layout (vertices = 4) out;

// varying input from vertex shader
in vec2 TexCoord[];
in vec3 Pos[];

// varying output to evaluation shader
out vec2 TextureCoord[];
out vec3 csPos[];

uniform int tessellationFactor;


void main()
{

	if (gl_InvocationID == 0)
	{
		gl_TessLevelInner[0] = tessellationFactor;
		gl_TessLevelInner[1] = tessellationFactor;

		gl_TessLevelOuter[0] = tessellationFactor;
		gl_TessLevelOuter[1] = tessellationFactor;
		gl_TessLevelOuter[2] = tessellationFactor;		
		gl_TessLevelOuter[3] = tessellationFactor;
	}

	csPos[gl_InvocationID] = Pos[gl_InvocationID];
	TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];
}