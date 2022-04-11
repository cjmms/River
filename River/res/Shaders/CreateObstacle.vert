#version 450 core
layout(Location = 0) in vec2 aPos;
layout(Location = 1) in vec2 aTextureCoord;

uniform mat4 transformationMatrix;

out vec2 texCoord;

void main()
{
	vec4 outputPos = transformationMatrix * vec4(aPos, 0, 1);

	gl_Position = vec4(outputPos.x, outputPos.y, 0.0, 1);

	texCoord = aTextureCoord;
}
