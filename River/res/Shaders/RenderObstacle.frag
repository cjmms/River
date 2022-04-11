#version 410 core

in vec2 TexCoord;
in vec3 worldPos;
in vec3 Normal;

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
    vec3 sunLight = vec3(1, 1, 1);
    vec3 sunPos = vec3(10, 10, 10);
    vec3 sunDir = normalize(sunPos - worldPos);

    float diff = max(dot(Normal, sunDir), 0.0);
    vec3 diffuse = diff * sunLight;

    vec3 objectColor = vec3(0.8, 0.7, 0.1);


    vec3 result = (vec3(0.01) + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);

    
}