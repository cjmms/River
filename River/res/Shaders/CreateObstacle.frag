#version 450 core

uniform sampler2D checkerboard;
uniform sampler2D roundObstacle;

in vec2 texCoord;

out vec4 FragColor;


void main()
{
	vec4 outputColor = vec4(texture(roundObstacle, texCoord).xyz, 1.0);

	if (outputColor.xyz == vec3(0, 0, 0)) discard;

	FragColor = vec4(1);
}
