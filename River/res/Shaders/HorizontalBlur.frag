#version 450 core

uniform sampler2D inputTexture;

in vec2 TexCoords;
out vec4 FragColor;


void main()
{
    FragColor = texture(inputTexture, TexCoords);
}