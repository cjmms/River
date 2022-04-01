#version 410 core
layout(location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTex;

out vec2 TexCoord;
out vec3 Pos;


void main()
{
	Pos = vec3(aPos.x, aPos.y, 0);
	TexCoord = aTex;
}
