#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;

void main()
{		    
    FragColor = vec4(texture(environmentMap, WorldPos).rgb, 1.0);
}