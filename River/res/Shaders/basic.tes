#version 450
layout(triangles, equal_spacing, ccw) in;



// received from Tessellation Control Shader - all texture coordinates for the patch vertices
in vec2 TextureCoord[];
in vec3 csPos[];

out vec2 TexCoord;
out float Height;

uniform sampler2D waveParticle;
uniform float heightFactor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;



void main()
{
	TexCoord = (gl_TessCoord.x * TextureCoord[0]) + (gl_TessCoord.y * TextureCoord[1]) + (gl_TessCoord.z * TextureCoord[2]); 	
	vec3 pos = (gl_TessCoord.x * csPos[0]) + (gl_TessCoord.y * csPos[1]) + (gl_TessCoord.z * csPos[2]); 


	float height = texture(waveParticle, TexCoord).r;

	gl_Position =  model * vec4(pos.xyz, 1.0);
	gl_Position.y += height * heightFactor;

	gl_Position = projection * view * gl_Position;

	Height = height;
}