#version 450 core

uniform sampler2D horiBlurMap;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

in vec2 TexCoords;
out vec4 FragColor;


void main()
{
    float texHeight = 1.0 / textureSize(horiBlurMap, 0).y; // gets size of single texel

    vec3 result = texture(horiBlurMap, TexCoords).rgb * weight[0]; // current fragment's contribution

    for(int i = 1; i < 5; ++i)
    {
        result += texture(horiBlurMap, TexCoords + vec2(0.0, texHeight * i)).rgb * weight[i];    // top
        result += texture(horiBlurMap, TexCoords - vec2(0.0, texHeight * i)).rgb * weight[i];    // bottom
    }

    FragColor = vec4(result, 1.0);
}