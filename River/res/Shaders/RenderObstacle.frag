#version 410 core

in vec2 TexCoord;
in vec3 worldPos;
//in vec3 Normal;

out vec4 FragColor;


uniform float waterDepth;
uniform float extinctionCoeff;
uniform vec3 waterBedColor;
uniform bool enableNormalMap;
uniform float FoamTurbulance;
uniform samplerCube  IrradianceMap;
uniform samplerCube  skybox;
uniform vec3 ViewPos;






void main()
{
    FragColor = vec4(1, 0, 0, 1);
}