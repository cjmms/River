#version 410 core

in vec2 TexCoord;
in vec3 worldPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D waveParticle;
uniform sampler2D checkerBoard;
uniform float waterDepth;
uniform float extinctionCoeff;
uniform vec3 waterBedColor;
uniform bool enableNormalMap;


// Exponential Integral
// (http://en.wikipedia.org/wiki/Exponential_integral)
float Ei( float z )
{
    const float EulerMascheroniConstant = 0.577216f;
    float z2 = z  * z;
    float z3 = z2 * z;
    float z4 = z3 * z;
    float z5 = z4 * z;
    return EulerMascheroniConstant + log(z) + z + z2/4.f + z3/18.f + z4/96.f + z5/600.f;
}

// _Position: current position in the water
// _ExtinctionCoeff: used for beer lambert law
// VolumeTop: shortest distance from _Position to water surface
// VolumeBottom: shortest distance from _Position to water surface
// IsotropicLightTop: uniform radiance from top hemisphere
// IsotropicLightBottom: uniform radiance from bottom hemisphere
vec3 ComputeAmbientColor (  vec3 _Position, float _ExtinctionCoeff, 
                            float VolumeTop, float VolumeBottom, 
                            vec3 IsotropicLightTop, vec3 IsotropicLightBottom)
{
    float Hp = VolumeTop - _Position.y; // Height to the top of the volume
    float a = -_ExtinctionCoeff * Hp;
    vec3 IsotropicScatteringTop = IsotropicLightTop * max( 0.0, exp( a ) - a * Ei( a ));
    float Hb = _Position.y - VolumeBottom; // Height to the bottom of the volume
    a = -_ExtinctionCoeff * Hb;
    vec3 IsotropicScatteringBottom = IsotropicLightBottom * max( 0.0, exp( a ) - a * Ei( a ));
    return IsotropicScatteringTop + IsotropicScatteringBottom;
}



void main()
{
    //FragColor = texture(waveParticle, TexCoord);
    vec3 sunLight = vec3(1, 1, 1);
  
    FragColor = texture(checkerBoard, TexCoord);

    float ratio = 0.5;

    // for now, let's choose mid point as _Position
    vec3 position = vec3(worldPos.x, worldPos.y - ratio * waterDepth, worldPos.z);
    float VolumeTop = ratio * waterDepth;
    float VolumeBottom = waterDepth - VolumeTop;

    vec3 IsotropicLightBottom = waterBedColor * exp(-waterDepth* extinctionCoeff);
    vec3 IsotropicLightTop = sunLight;

    vec3 AmbientColor = ComputeAmbientColor(position, extinctionCoeff, 
                                            VolumeTop, VolumeBottom, IsotropicLightTop, IsotropicLightBottom);

    FragColor = vec4(AmbientColor, 1);
    if (enableNormalMap)
    {
        FragColor = vec4(abs(Normal), 1);
    }
}