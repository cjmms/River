

#version 410 core

in vec2 TexCoord;
in float Height;

out vec4 FragColor;

uniform sampler2D waveParticle;
uniform sampler2D checkerBoard;

void main()
{
    //FragColor = texture(waveParticle, TexCoord);

    float h = Height;	// shift and scale the height in to a grayscale value
    FragColor = texture(checkerBoard, TexCoord);
}