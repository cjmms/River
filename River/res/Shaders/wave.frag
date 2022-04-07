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
uniform float FoamTurbulance;
uniform samplerCube  IrradianceMap;
uniform vec3 ViewPos;


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
    vec3 sunPos = vec3(10, 10, 10);
    vec3 sunDir = normalize(sunPos - worldPos);
    vec3 viewDir = normalize(ViewPos - worldPos);

    vec3 sunLight = vec3(1, 1, 1);
    vec3 waterColor = vec3(0, 0.3, 0.4);
  
    vec3 RiverBedPos = vec3(worldPos.x, waterDepth, worldPos.z);

    // for now, let's choose mid point as _Position
    vec3 HalfPoint = mix(RiverBedPos, worldPos, 0.5f + float(FoamTurbulance));

    vec3 IsotropicLightBottom = sunLight * waterBedColor * exp((HalfPoint.y - worldPos.y) * extinctionCoeff) * dot(sunDir, vec3(0, 1, 0));
    vec3 IsotropicLightTop = sunLight * waterColor * dot(sunDir, vec3(0, 1, 0));

    // scattering caused by sun light
    vec3 AmbientColor = ComputeAmbientColor(HalfPoint, extinctionCoeff, 
                                            worldPos.y, RiverBedPos.y, IsotropicLightTop, IsotropicLightBottom);

    // blinn phong specular
    // https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
    vec3 halfwayDir = normalize(sunDir + viewDir);

    float shininess = 32;
    float specular = pow(max(dot(Normal, halfwayDir), 0.0), shininess);

    vec3 SpecularColor = vec3(specular);

    //FragColor = vec4(SpecularColor, 1);

    //FragColor = vec4(AmbientColor + SpecularColor, 1);

    //FragColor = texture(checkerBoard, TexCoord);

    FragColor = vec4(AmbientColor, 1);

    // color from irradiance map
    vec3 IrradianceColor = vec3(texture(IrradianceMap, Normal)) * waterColor * 0.1;

    FragColor = vec4(AmbientColor + IrradianceColor, 1);

    if (enableNormalMap)
    {
        FragColor = vec4(abs(Normal), 1);
    }
}