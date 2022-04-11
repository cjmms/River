#version 450
layout(quads, equal_spacing, cw) in;

// received from Tessellation Control Shader - all texture coordinates for the patch vertices
in vec2 TextureCoord[];


out vec2 TexCoord;
out vec3 worldPos;
//out vec3 Normal;

uniform sampler2D ObstacleHeightMap;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float obstacleHeightFactor;





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

    // normal * heightmap
    // for now, assume normal is vec3(0, 1, 0)
    vec3 deviation = vec3(0, 1, 0) * texture(ObstacleHeightMap, TexCoord).x * obstacleHeightFactor;

    // apply height map
    worldPos = gl_Position.xyz + deviation;     // apply height map

	gl_Position = projection * view * vec4(worldPos, 1);   

    //Normal = GenerateNormal();
}