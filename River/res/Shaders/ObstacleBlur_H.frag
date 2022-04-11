#version 450 core

uniform sampler2D obstaclePosMap;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

in vec2 TexCoords;
out vec4 FragColor;


void main()
{
    float texWidth = 1.0 / textureSize(obstaclePosMap, 0).x; // gets size of single texel

    vec3 result = texture(obstaclePosMap, TexCoords).rgb * weight[0]; // current fragment's contribution

    for(int i = 1; i < 5; ++i)
    {
        result += texture(obstaclePosMap, TexCoords + vec2(texWidth * i, 0.0)).rgb * weight[i];    // right
        result += texture(obstaclePosMap, TexCoords - vec2(texWidth * i, 0.0)).rgb * weight[i];    // left
    }

    FragColor = vec4(result, 1.0);
}