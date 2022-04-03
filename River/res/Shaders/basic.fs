

#version 330 core

in vec2 TexCoord;
in float Height;

out vec4 FragColor;

uniform sampler2D waveParticle;

void main()
{
    //FragColor = texture(waveParticle, TexCoord);

    float h = Height;	// shift and scale the height in to a grayscale value
    FragColor = vec4(h, h, h, 1.0);
}