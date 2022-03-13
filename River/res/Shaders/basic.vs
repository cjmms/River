#version 330 core
layout(location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 TexCoord;
out vec3 Pos;


void main()
{
	Pos = aPos;
	TexCoord = aTex;
}
