#version 450
layout(quads, fractional_odd_spacing, ccw) in;



// received from Tessellation Control Shader - all texture coordinates for the patch vertices
in vec2 TextureCoord[];
in vec3 csPos[];

out vec2 TexCoord;
out float Height;
out vec3 Normal;

uniform sampler2D gradientMap;
uniform sampler2D deviationMap;

uniform float dxScale;
uniform float dzScale;
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

    gl_Position += vec4(deviation.x , deviation.y , deviation.z  , 0);

	gl_Position = projection * view * gl_Position;

	Height = deviation.y;

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

    Normal = normalize(cross(normalize(ddv), normalize(ddu)));
    //Normal = normalize(ddu);
    //Normal = normalize(right) - normalize(left);

    //vec3 normal = normalize(vec3(down - up, 2.0, left - right));

    //Normal = vec3(model * vec4(normal, 0));     // normal in world space
    //Normal = normal;
    //Normal =  vec3(deviation.x, deviation.y * heightFactor, deviation.z);
}