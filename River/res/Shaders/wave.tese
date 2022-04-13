#version 450
layout(quads, equal_spacing, cw) in;

// received from Tessellation Control Shader - all texture coordinates for the patch vertices
in vec2 TextureCoord[];


out vec2 TexCoord;
out vec3 worldPos;
out vec3 Normal;

uniform sampler2D gradientMap;
uniform sampler2D deviationMap;

uniform float time;
uniform float timeScale;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D obstacleMap;

uniform sampler2D flowMap;

// sample 4 near texels
// generate two tangents
// the cross product is the normal
// The presicion of normal highly depends on tessellation level
vec3 GenerateNormal()
{
    float offsetU = 1 / float(textureSize(deviationMap, 0).x);     // width of one texel
    float offsetV = 1 / float(textureSize(deviationMap, 0).y);     // height of one texel

    // access 4 near deviations
    vec3 left  = texture(deviationMap, TexCoord + vec2(-offsetU, 0.0)).xyz;
    vec3 right = texture(deviationMap, TexCoord + vec2( offsetU, 0.0)).xyz;
    vec3 up    = texture(deviationMap, TexCoord + vec2(0.0,  offsetV)).xyz;
    vec3 down  = texture(deviationMap, TexCoord + vec2(0.0, -offsetV)).xyz;

    vec3 ddu = right - left;    // difference of deviation in horizontal direction
    vec3 ddv = up - down;       // difference of deviation in vertical direction

    ddu += vec3(2 * offsetU, 0, 0);
    ddv += vec3(0, 0, 2 * offsetV);

    return normalize(cross(normalize(ddv), normalize(ddu)));
}



void main()
{
	// bilinearly interpolate texture coordinate across patch
    vec2 t0 = mix(TextureCoord[0], TextureCoord[1], gl_TessCoord.x);
    vec2 t1 = mix(TextureCoord[2], TextureCoord[3], gl_TessCoord.x);
    TexCoord = mix(t0, t1, gl_TessCoord.y);

    //bilinearly interpolate position coordinate across patch
    vec4 p0 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
    vec4 p1 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
    vec3 pos = mix(p0, p1, gl_TessCoord.y).xyz;


	gl_Position =  model * vec4(pos.xyz, 1.0);

    //vec2 velocity = texture(flowMap, TexCoord).xy * timeScale * 0.05;
    //velocity = vec2(2.0, 0.1);

    // apply height map
    worldPos = gl_Position.xyz + texture(deviationMap, TexCoord).xyz;

    vec2 velocity = texture(flowMap, TexCoord).xy * timeScale * 0.01;

    worldPos += texture(deviationMap, TexCoord - velocity).xyz;


	gl_Position = projection * view * vec4(worldPos, 1);   

    Normal = GenerateNormal();
}