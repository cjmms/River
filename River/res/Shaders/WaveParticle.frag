#version 450 core

in vec2 velocity;
in float amplitude;

out vec4 FragColor;



void main()
{
	FragColor = vec4(velocity, amplitude, 1);	// velocity.x, velocity.y, amplitude, 1
}
