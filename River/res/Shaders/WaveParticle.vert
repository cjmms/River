#version 450 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aDir;
layout (location = 2) in float aHeight;
layout (location = 3) in float aSpeed;
layout (location = 4) in float aRadius;


//out vec2 TexCoord;
//out vec3 Pos;


void main()
{
	gl_Position = vec4(aPos, 0.1, 1);
}
