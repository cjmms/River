#version 450 core

layout (location = 0) out vec4 out1;
layout (location = 1) out vec4 out2;

uniform int blurRadius;

uniform sampler2D tex1;     // x:f_1 y:f_2 z:f_3 
uniform sampler2D tex2;     // x:f_4 y:f_5 z:velX w:vel

uniform float dxScale;
uniform float dzScale;
uniform float heightFactor;

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
    vec3 f123 = texture(tex1, TexCoords).rgb;
    vec4 f45v = texture(tex2, TexCoords);

    vec4 deviation = vec4(f45v.x, 0, f123.x, 1);
    vec4 gradient = vec4(f123.y, 0, 0, 1);
    vec2 gradCorr = vec2(f123.z, f45v.y);       // gradient correction

    float textureHeight = textureSize(tex1, 0).y;

    for (int i = 1; i <= blurRadius; i++)
    {
        float offset = i / float(textureHeight);

        vec4 f123B = texture(tex1, TexCoords + vec2(0, offset));
        vec4 f123T = texture(tex1, TexCoords + vec2(0, -offset));

        vec4 f45vB = texture(tex2, TexCoords + vec2(0, offset));
        vec4 f45vT = texture(tex2, TexCoords + vec2(0, -offset));

        vec3 f = GetFilter(i / float(blurRadius));

        deviation.x += (f45vB.x + f45vT.x) * f.x * f.x;         // deviation X
        deviation.y += (f45vB.y - f45vT.y) * 2 * f.x * f.y;     // deviation Y
        deviation.z += (f123B.x + f123T.x) * f.x;               // deviation Z
        gradient.x += (f123B.y + f123T.y) * f.x;                // gradient X
        gradient.y += (f123B.x - f123T.x) * f.y;                // gradient Y
        gradCorr.x += (f123B.z + f123T.z) * f.x * f.x;          // gradient X horizontal deviation
        gradCorr.y += (f45vB.y + f45vT.y) * f.z;                // gradient Y horizontal deviation

    }
     
    // fix gradient considering horizontal deviation
    gradCorr *= PI / blurRadius;
    gradient.xy *= (PI / blurRadius) / (1 + gradCorr);

   
    //out1 = vec4(deviation.x * dxScale, deviation.y * heightFactor, deviation.z * dzScale, deviation.w);

    deviation.x *= dxScale;
    deviation.y *= dzScale;
    out1 = vec4(-deviation.x, deviation.z, -deviation.y, deviation.w);

    //out1 = vec4(deviation.x * dxScale, deviation.y * heightFactor, deviation.z * dzScale, deviation.w);
    //out1 = deviation;
    out2 = gradient;
}