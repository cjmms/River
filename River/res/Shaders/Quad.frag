#version 450 core

uniform sampler2D particleMap;
uniform sampler2D f123;
uniform sampler2D f45v;
uniform sampler2D deviation;
uniform sampler2D gradient;
uniform sampler2D waveMesh;
uniform sampler2D obstacleMap;
uniform sampler2D blurredObstacleMap;

uniform sampler2D flowDivergence;
uniform sampler2D flowVelocity;
uniform sampler2D flowPressure;

uniform int selectedRenderPass;

in vec2 TexCoords;
out vec4 FragColor;


void main()
{
    if (selectedRenderPass == 0)        // particle velocity
    {
        FragColor = vec4(6 * texture(particleMap, TexCoords).xy, 0, 1);
    }
    else if (selectedRenderPass == 1)    // particle amplitude
    {
        FragColor = vec4(0, texture(particleMap, TexCoords).z, 0, 1);
    }
    else if (selectedRenderPass == 2)   // "Horizontal Pass"
    {
        FragColor = vec4(4 * texture(f45v, TexCoords).zw, 0, 1);
    }
    else if (selectedRenderPass == 3)   // "Wave Height Map(Deviation)"
    {
        FragColor = vec4(texture(deviation, TexCoords).xyz, 1);
    }
    else if (selectedRenderPass == 4)   // "Wave Height Map(Gradient)"
    {
        FragColor = vec4(5 * texture(gradient, TexCoords).xyz, 1);
    }
    else if (selectedRenderPass == 5)   // "Wave Mesh"
    {
        FragColor = texture(waveMesh, TexCoords);
    }
    else if (selectedRenderPass == 6)
    {
        FragColor = texture(obstacleMap, TexCoords);
    }
    else if (selectedRenderPass == 7)
    {
        FragColor = texture(blurredObstacleMap, TexCoords);
    }
    else if (selectedRenderPass == 8) // Flow divergence
    {
        FragColor= abs(texture(flowDivergence, TexCoords));
    }
    else if (selectedRenderPass == 9) // Flow velocity
    {
        FragColor = abs(texture(flowVelocity, TexCoords));
    }
    else if (selectedRenderPass == 10) // Flow pressure
    {
        FragColor = abs(texture(flowPressure, TexCoords));
    }

}
