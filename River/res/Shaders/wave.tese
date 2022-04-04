#version 450
layout(quads, fractional_odd_spacing, ccw) in;



// received from Tessellation Control Shader - all texture coordinates for the patch vertices
in vec2 TextureCoord[];
in vec3 csPos[];

out vec2 TexCoord;
out vec3 worldPos;

uniform sampler2D gradientMap;
uniform sampler2D deviationMap;
uniform float heightFactor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;



void main()
{
	// get patch coordinate
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

	// ----------------------------------------------------------------------
    // retrieve control point texture coordinates
    vec2 t00 = TextureCoord[0];
    vec2 t01 = TextureCoord[1];
    vec2 t10 = TextureCoord[2];
    vec2 t11 = TextureCoord[3];

	// bilinearly interpolate texture coordinate across patch
    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    TexCoord = (t1 - t0) * v + t0;	    // interpolated UV
	//-----------------------------------------------------------------------

	// ----------------------------------------------------------------------
    // retrieve control point position coordinates
    vec3 p00 = csPos[0];
    vec3 p01 = csPos[1];
    vec3 p10 = csPos[2];
    vec3 p11 = csPos[3];

    // bilinearly interpolate position coordinate across patch
    vec3 p0 = (p01 - p00) * u + p00;
    vec3 p1 = (p11 - p10) * u + p10;
    vec3 pos = (p1 - p0) * v + p0;      // interpolated UV
    //------------------------------------------------------------------------

	vec3 deviation = texture(deviationMap, TexCoord).xyz;

	gl_Position =  model * vec4(pos.xyz, 1.0);

	gl_Position.x += deviation.x;
	gl_Position.y += deviation.y * heightFactor;
	gl_Position.z += deviation.z;

    worldPos = gl_Position.xyz;

	gl_Position = projection * view * gl_Position;   
}