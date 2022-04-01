#version 450 core

layout (location = 0) out vec4 out1;
layout (location = 1) out vec4 out2;

uniform sampler2D inputTexture;
uniform int blurRadius;

in vec2 TexCoords;

float PI = 3.14159265359f;


// part of wave generation filtering
// source: http://www.cemyuksel.com/research/waveparticles/cem_yuksel_dissertation.pdf
// page 208
vec3 GetFilter(float v)
{
    float s = sin(PI * v);
    float c = cos(PI * v);

    return vec3(
        0.5f * (c + 1.0f),              // 0.5 ( cos(v) + 1 )
        -0.5f * s,                      // -0.5 sin(v)
        -0.25f * (c * c - s * s + c)    // cos(2v) + cos(v)
    );
}


void main()
{ 
    vec3 velAmp = texture(inputTexture, TexCoords).rgb;         // x:velX y:velY z:amplitude
    vec4 f123 = vec4(velAmp.z, 0, 0.5f * velAmp.z, 1);          // x:f_1 y:f_2 z:f_3 (times amplitude)
    vec4 f45v = vec4(0, velAmp.z, sign(velAmp.z)*velAmp.xy );   // x:f_4 y:f_5 z:velX w:velY

    float textureWidth = textureSize(inputTexture, 0).x;

    for (int i = 1; i <= blurRadius; ++i)
    {
        float offset = i / textureWidth;

        vec4 velAmpL = texture(inputTexture, TexCoords + vec2(-offset, 0));   // sample left texels
        vec4 velAmpR = texture(inputTexture, TexCoords + vec2(offset, 0));    // sample right texels

        float ampSum = velAmpL.z + velAmpR.z;
        float ampDif = velAmpL.z - velAmpR.z;

        vec3 f = GetFilter(i / float(blurRadius)) ;

        f123.x += ampSum * f.x;
        f123.y += ampDif * f.y;
        f123.z += ampSum * f.z;

        f45v.x += ampDif * f.x * f.y * 2;
        f45v.y += ampSum * f.x * f.x;
            
        f45v.z += (sign(velAmpL.z) * velAmpL.x + sign(velAmpR.z) * velAmpR.x) * f.x;
        f45v.w += (sign(velAmpL.z) * velAmpL.y + sign(velAmpR.z) * velAmpR.y) * f.x;
    }

    out1 = f123;
    out2 = f45v;
}